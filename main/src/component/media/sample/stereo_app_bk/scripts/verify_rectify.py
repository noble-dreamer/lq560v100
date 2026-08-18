#!/usr/bin/env python3
"""
棋盘格畸变矫正验证工具

功能:
1. 从板端接收左/右图像 (JPEG)
2. 检测棋盘格角点
3. 单目: 对比板端LDC矫正后 vs OpenCV软件矫正后 的角点行直线度
4. 双目: 测量左右图像同名角点的 Y 轴视差 (epipolar alignment / Y-disparity)
5. 判断LDC是否生效、标定参数是否正确、双目基线是否对齐

使用:
  python3 verify_rectify.py [--host 192.168.1.101] [--port 9000]
  python3 verify_rectify.py --image saved_left.jpg   # 离线单目模式
  python3 verify_rectify.py --left-image L.jpg --right-image R.jpg  # 离线双目模式
  python3 verify_rectify.py --save  # 在线模式并保存结果图像

棋盘格: 实拍靶标按 9×6 内角点检测, 25mm间距, 距相机约30cm
"""

import argparse
import json
import socket
import struct
import sys
import os
import numpy as np
import cv2
from typing import Optional, Tuple, Dict

# ── 协议常量 ──
MAGIC = 0x53544D50
HEADER_FMT = '>IBIQI3s'
HEADER_SIZE = struct.calcsize(HEADER_FMT)

# 棋盘格参数
CHESS_ROWS = 6   # 内角点行数
CHESS_COLS = 9   # 内角点列数
SQUARE_SIZE_MM = 25.0

# 标定文件路径
CALIB_PATH = os.path.join(os.path.dirname(__file__), 'stereo_calib.json')


def recv_exact(sock, n):
    data = b''
    while len(data) < n:
        chunk = sock.recv(n - len(data))
        if not chunk:
            return None
        data += chunk
    return data


def grab_frame_pair(host, port, timeout=10):
    """从板端接收一组完整帧 (left + right)"""
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(timeout)
    print(f"[verify] 连接 {host}:{port} ...")
    sock.connect((host, port))
    print("[verify] 已连接, 等待帧数据...")

    frames = {}
    while len(frames) < 2:  # 至少获取 left + right
        header_raw = recv_exact(sock, HEADER_SIZE)
        if header_raw is None:
            break
        magic, ftype, frm_idx, ts_us, payload_size, _ = struct.unpack(HEADER_FMT, header_raw)
        if magic != MAGIC:
            continue
        payload = recv_exact(sock, payload_size)
        if payload is None:
            break

        if ftype == 0:  # left
            img = cv2.imdecode(np.frombuffer(payload, np.uint8), cv2.IMREAD_COLOR)
            if img is not None:
                frames['left'] = img
                print(f"  收到 left: {img.shape[1]}x{img.shape[0]}")
        elif ftype == 1:  # right
            img = cv2.imdecode(np.frombuffer(payload, np.uint8), cv2.IMREAD_COLOR)
            if img is not None:
                frames['right'] = img
                print(f"  收到 right: {img.shape[1]}x{img.shape[0]}")
        # skip disparity frames

    sock.close()
    return frames


def load_calibration(calib_path):
    """加载标定参数, 返回 (cam_matrix_L, dist_L, cam_matrix_R, dist_R, image_size)"""
    with open(calib_path, 'r') as f:
        calib = json.load(f)

    img_size = tuple(calib['image_size'])  # [W, H]

    def parse_cam(obj):
        K = np.array([
            [obj['fx'], 0, obj['cx']],
            [0, obj['fy'], obj['cy']],
            [0, 0, 1]
        ], dtype=np.float64)
        dist = np.array(obj['distortion'], dtype=np.float64)
        return K, dist

    K_L, dist_L = parse_cam(calib['left'])
    K_R, dist_R = parse_cam(calib['right'])

    return K_L, dist_L, K_R, dist_R, img_size, calib


def measure_line_straightness(corners, cols):
    """
    测量棋盘格角点的行直线度 (epipolar alignment proxy).
    对每一行角点做线性拟合, 返回平均残差(像素).
    """
    rows = corners.shape[0] // cols
    residuals = []
    for r in range(rows):
        pts = corners[r * cols: (r + 1) * cols, 0, :]  # [cols, 2]
        # 线性拟合 y = a*x + b
        x = pts[:, 0]
        y = pts[:, 1]
        if np.std(x) < 1e-6:
            continue
        coeffs = np.polyfit(x, y, 1)
        fitted_y = np.polyval(coeffs, x)
        res = np.abs(y - fitted_y)
        residuals.extend(res.tolist())
    return np.mean(residuals) if residuals else float('inf')


def detect_corners(img, label: str):
    """检测棋盘格角点并亚像素精化, 返回 (corners [N,1,2] float32, None if failed)"""
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    pattern_size = (CHESS_COLS, CHESS_ROWS)
    flags = cv2.CALIB_CB_ADAPTIVE_THRESH + cv2.CALIB_CB_NORMALIZE_IMAGE + cv2.CALIB_CB_FILTER_QUADS
    ret, corners = cv2.findChessboardCorners(gray, pattern_size, flags)
    if not ret:
        ret, corners = cv2.findChessboardCornersSB(gray, pattern_size, cv2.CALIB_CB_NORMALIZE_IMAGE)
    if not ret:
        print(f"  [{label}] ⚠ 未检测到棋盘格角点!")
        return None
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
    corners = cv2.cornerSubPix(gray, corners, (11, 11), (-1, -1), criteria)
    return corners


def detect_and_analyze(img, K, dist, label, img_size):
    """检测棋盘格并分析单目畸变矫正效果"""
    # 检测原始图角点
    corners = detect_corners(img, label)
    if corners is None:
        return None, None, None

    # 测量原始图像角点行直线度
    raw_straightness = measure_line_straightness(corners, CHESS_COLS)

    # OpenCV 去畸变
    h, w = img.shape[:2]
    new_K, roi = cv2.getOptimalNewCameraMatrix(K, dist, (w, h), 1, (w, h))
    undistorted = cv2.undistort(img, K, dist, None, new_K)

    # 在去畸变图像上重新检测角点
    gray_ud = cv2.cvtColor(undistorted, cv2.COLOR_BGR2GRAY)
    ret_ud, corners_ud = cv2.findChessboardCorners(
        gray_ud, (CHESS_COLS, CHESS_ROWS),
        cv2.CALIB_CB_ADAPTIVE_THRESH + cv2.CALIB_CB_NORMALIZE_IMAGE + cv2.CALIB_CB_FILTER_QUADS)
    if not ret_ud:
        ret_ud, corners_ud = cv2.findChessboardCornersSB(
            gray_ud, (CHESS_COLS, CHESS_ROWS), cv2.CALIB_CB_NORMALIZE_IMAGE)

    ud_straightness = float('inf')
    if ret_ud:
        criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
        corners_ud = cv2.cornerSubPix(gray_ud, corners_ud, (11, 11), (-1, -1), criteria)
        ud_straightness = measure_line_straightness(corners_ud, CHESS_COLS)

    print(f"  [{label}] 板端图像角点行直线度: {raw_straightness:.3f} px")
    print(f"  [{label}] OpenCV去畸变后直线度: {ud_straightness:.3f} px")

    if raw_straightness < 0.5:
        print(f"  [{label}] ✓ 板端图像已矫正 (直线度<0.5px)")
    elif ud_straightness < 0.5:
        print(f"  [{label}] ✗ 板端未矫正, 但标定数据正确 (OpenCV可矫正)")
        print(f"  [{label}]   → 问题在板端LDC硬件/参数配置")
    else:
        print(f"  [{label}] ✗ 标定数据可能有问题 (OpenCV也无法矫正到<0.5px)")

    # 绘制结果
    vis_raw = img.copy()
    cv2.drawChessboardCorners(vis_raw, (CHESS_COLS, CHESS_ROWS), corners, True)
    cv2.putText(vis_raw, f"Raw straightness: {raw_straightness:.3f}px",
                (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)

    vis_ud = undistorted.copy()
    if ret_ud:
        cv2.drawChessboardCorners(vis_ud, (CHESS_COLS, CHESS_ROWS), corners_ud, True)
    cv2.putText(vis_ud, f"OpenCV undistort straightness: {ud_straightness:.3f}px",
                (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)

    return vis_raw, vis_ud, {'raw': raw_straightness, 'undistorted': ud_straightness}


# ─────────────────────────────────────────────────────────────────────────────
# 双目基线对齐 (Y-disparity / epipolar error) 测量
# ─────────────────────────────────────────────────────────────────────────────

def analyze_stereo_alignment(img_left, img_right, calib: dict, save_prefix: str = None):
    """
    测量板端输出图像的双目极线对齐误差 (Y-disparity)。

    当前 stereo_app 已在板端通过 VPROC XY-LUT 完成 undistort + rectify，
    因此这里直接在板端输出图上检测左右同名角点并计算 y_left - y_right，
    不能再用 OpenCV 对图像做第二次 undistort/rectify。
    """
    print("  [stereo] 在板端输出图像上检测角点...")
    corners_L = detect_corners(img_left, "Left-board")
    corners_R = detect_corners(img_right, "Right-board")

    if corners_L is None or corners_R is None:
        print("  [stereo] ✗ 无法在板端输出图像上检测到角点, 跳过双目分析")
        return None

    pts_L = corners_L.reshape(-1, 2)
    pts_R = corners_R.reshape(-1, 2)
    n = min(len(pts_L), len(pts_R))
    if n < CHESS_ROWS * CHESS_COLS:
        print(f"  [stereo] ⚠ 角点数量不一致 (L={len(pts_L)}, R={len(pts_R)}), 取前 {n} 个")
    pts_L = pts_L[:n]
    pts_R = pts_R[:n]

    dy = pts_L[:, 1] - pts_R[:, 1]
    dx = pts_L[:, 0] - pts_R[:, 0]

    mean_dy = float(np.mean(np.abs(dy)))
    max_dy  = float(np.max(np.abs(dy)))
    std_dy  = float(np.std(dy))
    mean_dx = float(np.mean(dx))

    print(f"  [stereo] Y-disparity (板端输出极线对齐误差):")
    print(f"    均值 |dy| = {mean_dy:.3f} px  (目标 < 1.0 px)")
    print(f"    最大 |dy| = {max_dy:.3f} px")
    print(f"    标准差 dy = {std_dy:.3f} px")
    print(f"    均值 dx   = {mean_dx:.1f} px  (仅供参考, 应为正值)")

    if mean_dy < 1.0:
        print(f"  [stereo] ✓ 双目基线对齐良好 (mean|dy|<1.0px)")
    elif mean_dy < 3.0:
        print(f"  [stereo] △ 基线对齐一般 (1px < mean|dy| < 3px), 建议优化")
    else:
        print(f"  [stereo] ✗ 基线对齐较差 (mean|dy| >= 3px), 需要继续检查 LUT/标定")

    vis_epi = _draw_epipolar_vis(img_left, img_right, pts_L, pts_R, dy, mean_dy)

    if save_prefix:
        cv2.imwrite(f"{save_prefix}_board_left.jpg",  img_left)
        cv2.imwrite(f"{save_prefix}_board_right.jpg", img_right)
        cv2.imwrite(f"{save_prefix}_epipolar.jpg",    vis_epi)
        print(f"  [stereo] 已保存: {save_prefix}_board_left.jpg, *_right.jpg, *_epipolar.jpg")

    return {
        'mean_dy': mean_dy,
        'max_dy':  max_dy,
        'std_dy':  std_dy,
        'mean_dx': mean_dx,
        'n_points': n,
        'dy_per_point': dy.tolist(),
        'vis_epi': vis_epi,
        'rect_left':  img_left,
        'rect_right': img_right,
    }


def _draw_epipolar_vis(rect_L, rect_R, pts_L, pts_R, dy, mean_dy):
    """
    绘制极线对齐可视化图:
    - 左右矫正图像并排显示
    - 每个角点对用水平线连接 (绿=对齐好, 红=偏差大)
    - 绘制若干条等间距水平极线
    """
    h, w = rect_L.shape[:2]
    canvas = np.zeros((h, w * 2, 3), dtype=np.uint8)
    canvas[:, :w]  = rect_L
    canvas[:, w:]  = rect_R

    # 绘制等间距水平极线 (每 60px 一条)
    for y_epi in range(0, h, 60):
        cv2.line(canvas, (0, y_epi), (w * 2, y_epi), (80, 80, 80), 1)

    # 绘制角点连接线和标记
    for i, (pL, pR, d) in enumerate(zip(pts_L, pts_R, dy)):
        xL, yL = int(pL[0]),       int(pL[1])
        xR, yR = int(pR[0]) + w,   int(pR[1])
        abs_d = abs(d)
        # 颜色: 绿(<1px) → 黄(1~3px) → 红(>3px)
        if abs_d < 1.0:
            color = (0, 220, 0)
        elif abs_d < 3.0:
            color = (0, 200, 200)
        else:
            color = (0, 0, 220)
        # 连线
        cv2.line(canvas, (xL, yL), (xR, yR), color, 1)
        # 标记点
        cv2.circle(canvas, (xL, yL), 4, color, -1)
        cv2.circle(canvas, (xR, yR), 4, color, -1)
        # 在右图角点旁标注 dy 值
        cv2.putText(canvas, f"{d:+.1f}", (xR + 5, yR),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.35, color, 1)

    # 标题
    cv2.putText(canvas, f"Epipolar Alignment  mean|dy|={mean_dy:.2f}px",
                (10, 25), cv2.FONT_HERSHEY_SIMPLEX, 0.8,
                (255, 255, 255), 2)
    cv2.putText(canvas, "LEFT (rectified)",
                (10, h - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (200, 200, 200), 1)
    cv2.putText(canvas, "RIGHT (rectified)",
                (w + 10, h - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (200, 200, 200), 1)
    cv2.putText(canvas, "Green:<1px  Cyan:1~3px  Red:>3px",
                (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (180, 180, 180), 1)
    return canvas


def main():
    parser = argparse.ArgumentParser(description='棋盘格畸变矫正与双目基线对齐验证')
    parser.add_argument('--host', default='192.168.1.101')
    parser.add_argument('--port', type=int, default=9000)
    parser.add_argument('--image', help='离线单目模式: 使用已保存的左图像文件')
    parser.add_argument('--left-image',  help='离线双目模式: 左图像文件')
    parser.add_argument('--right-image', help='离线双目模式: 右图像文件')
    parser.add_argument('--calib', default=CALIB_PATH, help='标定文件路径')
    parser.add_argument('--save', action='store_true', help='保存结果图像')
    parser.add_argument('--no-display', action='store_true', help='不显示窗口 (适合无GUI环境)')
    args = parser.parse_args()

    # 加载标定
    print(f"[verify] 加载标定文件: {args.calib}")
    if not os.path.exists(args.calib):
        print(f"[verify] 错误: 标定文件不存在: {args.calib}")
        sys.exit(1)

    K_L, dist_L, K_R, dist_R, img_size, calib = load_calibration(args.calib)
    print(f"[verify] 标定图像尺寸: {img_size}")
    print(f"[verify] 左相机: fx={K_L[0,0]:.1f} fy={K_L[1,1]:.1f} cx={K_L[0,2]:.1f} cy={K_L[1,2]:.1f}")
    print(f"[verify] 左畸变: k1={dist_L[0]:.4f} k2={dist_L[1]:.4f} k3={dist_L[4]:.4f}")
    print(f"[verify] 右相机: fx={K_R[0,0]:.1f} fy={K_R[1,1]:.1f} cx={K_R[0,2]:.1f} cy={K_R[1,2]:.1f}")
    print(f"[verify] 右畸变: k1={dist_R[0]:.4f} k2={dist_R[1]:.4f} k3={dist_R[4]:.4f}")
    print()

    # ── 离线双目模式 ──
    if args.left_image and args.right_image:
        img_L = cv2.imread(args.left_image)
        img_R = cv2.imread(args.right_image)
        if img_L is None or img_R is None:
            print(f"[verify] 错误: 无法读取图像")
            sys.exit(1)
        print(f"[verify] 离线双目: L={img_L.shape[1]}x{img_L.shape[0]}, R={img_R.shape[1]}x{img_R.shape[0]}")

        print("\n=== 单目畸变分析 ===")
        vis_L_raw, vis_L_ud, stats_L = detect_and_analyze(img_L, K_L, dist_L, "Left",  img_size)
        vis_R_raw, vis_R_ud, stats_R = detect_and_analyze(img_R, K_R, dist_R, "Right", img_size)

        print("\n=== 双目基线对齐分析 (Y-disparity) ===")
        save_pfx = os.path.join(os.path.dirname(__file__), "verify_stereo") if args.save else None
        stereo_result = analyze_stereo_alignment(img_L, img_R, calib, save_prefix=save_pfx)

        _print_summary(stats_L, stats_R, stereo_result)

        if not args.no_display and stereo_result is not None:
            vis = stereo_result['vis_epi']
            scale = min(1.0, 1600 / max(vis.shape[:2]))
            cv2.imshow("Epipolar Alignment", cv2.resize(vis, None, fx=scale, fy=scale))
            print("\n按任意键关闭窗口...")
            cv2.waitKey(0)
            cv2.destroyAllWindows()
        return

    # ── 离线单目模式 ──
    if args.image:
        img = cv2.imread(args.image)
        if img is None:
            print(f"[verify] 错误: 无法读取图像 {args.image}")
            sys.exit(1)
        print(f"[verify] 离线图像: {img.shape[1]}x{img.shape[0]}")
        print("\n=== 左相机分析 ===")
        vis_raw, vis_ud, stats = detect_and_analyze(img, K_L, dist_L, "Left", img_size)
        if vis_raw is not None:
            if args.save:
                cv2.imwrite("verify_raw.jpg", vis_raw)
                cv2.imwrite("verify_undistort.jpg", vis_ud)
            if not args.no_display:
                cv2.imshow("Board Output (Raw from LDC)", vis_raw)
                cv2.imshow("OpenCV Undistort", vis_ud)
                cv2.waitKey(0)
        return

    # ── 在线模式: 从板端接收 ──
    frames = grab_frame_pair(args.host, args.port)
    if not frames:
        print("[verify] 错误: 未接收到帧")
        sys.exit(1)

    results = {}
    for side, (K, dist) in [('left', (K_L, dist_L)), ('right', (K_R, dist_R))]:
        if side not in frames:
            continue
        print(f"\n=== {side.upper()} 相机分析 ===")
        vis_raw, vis_ud, stats = detect_and_analyze(
            frames[side], K, dist, side, img_size)
        results[side] = stats
        if vis_raw is not None and args.save:
            cv2.imwrite(f"verify_{side}_raw.jpg", vis_raw)
            cv2.imwrite(f"verify_{side}_undistort.jpg", vis_ud)

    stereo_result = None
    if 'left' in frames and 'right' in frames:
        print("\n=== 双目基线对齐分析 (Y-disparity) ===")
        save_pfx = os.path.join(os.path.dirname(__file__), "verify_stereo") if args.save else None
        stereo_result = analyze_stereo_alignment(
            frames['left'], frames['right'], calib, save_prefix=save_pfx)

    _print_summary(results.get('left'), results.get('right'), stereo_result)

    # 显示结果
    if not args.no_display:
        for side, (K, dist) in [('left', (K_L, dist_L)), ('right', (K_R, dist_R))]:
            if side not in frames or not results.get(side):
                continue
            vis_raw, vis_ud, _ = detect_and_analyze(frames[side], K, dist, side.capitalize(), img_size)
            if vis_raw is not None:
                scale = min(1.0, 800 / max(vis_raw.shape[:2]))
                combined = np.hstack([
                    cv2.resize(vis_raw, None, fx=scale, fy=scale),
                    cv2.resize(vis_ud,  None, fx=scale, fy=scale)
                ])
                cv2.imshow(f"{side.capitalize()}: Board | OpenCV Undistort", combined)

        if stereo_result is not None:
            vis = stereo_result['vis_epi']
            scale = min(1.0, 1600 / max(vis.shape[:2]))
            cv2.imshow("Epipolar Alignment", cv2.resize(vis, None, fx=scale, fy=scale))

        print("\n按任意键关闭窗口...")
        cv2.waitKey(0)
        cv2.destroyAllWindows()


def _print_summary(stats_L, stats_R, stereo_result):
    print("\n" + "=" * 60)
    print("验证总结:")
    print("=" * 60)

    for side, stats in [("left", stats_L), ("right", stats_R)]:
        if stats is None:
            print(f"  {side}: 未检测到棋盘格")
            continue
        raw_ok = stats['raw'] < 0.5
        ud_ok  = stats['undistorted'] < 0.5
        if raw_ok:
            status = "✓ LDC矫正正常"
        elif ud_ok:
            status = "✗ LDC未生效, 标定数据正确 → 需修复板端LDC"
        else:
            status = "✗ 标定数据有问题 → 需重新标定"
        print(f"  {side}: {status}")
        print(f"         板端直线度={stats['raw']:.3f}px, OpenCV直线度={stats['undistorted']:.3f}px")

    if stereo_result is not None:
        m = stereo_result['mean_dy']
        mx = stereo_result['max_dy']
        if m < 1.0:
            epi_status = "✓ 双目基线对齐良好"
        elif m < 3.0:
            epi_status = "△ 基线对齐一般, 建议极线矫正"
        else:
            epi_status = "✗ 基线对齐较差, 需要极线矫正"
        print(f"  stereo: {epi_status}")
        print(f"          mean|dy|={m:.3f}px  max|dy|={mx:.3f}px  n={stereo_result['n_points']}角点")


if __name__ == '__main__':
    main()
