#!/usr/bin/env python3
"""
generate_xylut.py — 基于 stereo_calib.json 生成 SD3589C VPROC GDC 2D-LUT 文件

功能:
  将 undistort(去畸变) + rectify(极线矫正) 合并到一张 2D 查找表中。
  生成的 .bin 文件可直接加载为 VPROC GDC xylut 参数，替代 LDC + PMF 方案。

硬件管线顺序 (SD3589C):
  VI(1080×1280) → GDC/LUT(1080×1280) → grp_set_crop(1080×1280,offset0) → rotation270°(1280×1080)

LUT 工作空间: 传感器原生 1080×1280 (GDC 之前)
坐标系约定:
  - 输出坐标 (dst): 传感器原生 1080×1280 空间
  - 源坐标   (src): 同一空间内的原始畸变采样点

原理:
  对每个输出网格点 (xd, yd) [native 1080×1280 空间]:
    1. 转换到矫正后图像空间 (1280×1080): x_cal = yd,  y_cal = (1080-1) - xd
    2. 逆投影到归一化坐标:               [xn,yn,1] = P^{-1} * [x_cal, y_cal, 1]^T
    3. 应用逆矫正旋转:                    [xr,yr,1] = R^{-1} * [xn,yn,1]^T
    4. 应用原始畸变 (径向 k1,k2,k3):      r²=xr²+yr², rad=1+k1r²+k2r⁴+k3r⁶
    5. 投影到原始图像坐标:                xs = fx*xr*rad + cx,  ys = fy*yr*rad + cy
    6. 转换 xs,ys 回 native 1080×1280 空间

二进制文件格式 (与 ot_vproc_gdc_2dlut_head 一致):
  Header (100 bytes, packed):
    int32 mesh_width   = dst_w / cell_size
    int32 mesh_height  = dst_h / cell_size
    int32 cell_width   = cell_size
    int32 cell_height  = cell_size
    int32 dst_width    = 1080
    int32 dst_height   = 1280
    int8  reserved[76] = 0
  LUT data (grid_w × grid_h × 8 bytes):
    per point: uint32 src_x_q4, uint32 src_y_q4  (Q28.4 定点, 坐标×16, 小端)
    layout: row-major, grid 从 (0,0) 到 (dst_w, dst_h)

用法:
  python3 generate_xylut.py [--calib stereo_calib.json] [--cell 16]
                            [--out-left lut_left.bin] [--out-right lut_right.bin]
                            [--verify]
"""

import argparse
import json
import os
import struct
import sys
import numpy as np
import cv2

# ── 硬件常量 ──────────────────────────────────────────────────────────────────
# GDC 处理全传感器帧
NATIVE_W = 1080   # 传感器原生宽
NATIVE_H = 1280   # 传感器原生高

# grp_set_crop 在 GDC 之后: 保留全幅 1080×1280 (left offset = 0)
CROP_OFFSET_X = 0
CROP_W = 1080     # STEREO_SENSOR_HEIGHT

# rotation 270° CW 后: 1280×1080
CAL_W = 1280      # 标定图像宽 (rotated)
CAL_H = 1080      # 标定图像高 (rotated)

# LUT header size (packed, no padding)
LUT_HEADER_SIZE = 24 + 76  # 6 × int32 + 76 reserved = 100 bytes


# ── 坐标变换 ──────────────────────────────────────────────────────────────────

def native_to_cal(xn, yn):
    """
    native 1080×1280 → calibrated 1280×1080 坐标变换
    270° CW rotation of (1080×1280 after crop):
        x_cal = y_native
        y_cal = (CROP_W - 1 + CROP_OFFSET_X) - x_native
    """
    x_cal = yn
    y_cal = (CROP_W - 1 + CROP_OFFSET_X) - xn
    return x_cal, y_cal


def cal_to_native(xc, yc):
    """
    calibrated 1280×1080 → native 1080×1280 坐标变换 (逆变换)
        x_native = (CROP_W - 1 + CROP_OFFSET_X) - y_cal
        y_native = x_cal
    """
    x_nat = (CROP_W - 1 + CROP_OFFSET_X) - yc
    y_nat = xc
    return x_nat, y_nat


# ── LUT 计算 ─────────────────────────────────────────────────────────────────

def compute_lut(K_raw, dist, R_rect, P_rect, cell_size=16):
    """
    计算一个相机的 XY-LUT。

    输入:
      K_raw    : 原始相机内参矩阵 (3×3), 标定空间 1280×1080
      dist     : 畸变系数 [k1, k2, p1, p2, k3]
      R_rect   : stereoRectify 输出的矫正旋转矩阵 (3×3)
      P_rect   : stereoRectify 输出的投影矩阵 (3×4 或 3×3), 矫正后空间内参
      cell_size: 网格单元大小 (16/32/64)

    输出:
      lut_data : numpy array, shape=(grid_h, grid_w, 2), dtype=float32
                 lut_data[gy, gx] = (src_x_native, src_y_native)
                 坐标为传感器原生 1080×1280 空间的采样点
      grid_w, grid_h : 网格尺寸
    """
    # 目标空间: native 1080x1280
    # 驱动要求 dst_w 和 dst_h 必须能被 cell_size 整除.
    # 1080 % 16 = 8 不整除, 需要对齐到 1088.
    # 1280 % 16 = 0 已整除, 保持 1280.
    import math
    dst_w = math.ceil(NATIVE_W / cell_size) * cell_size   # 1088 (16-aligned)
    dst_h = math.ceil(NATIVE_H / cell_size) * cell_size   # 1280 (already aligned)

    # 网格点数
    # 官方 2D-LUT 文件的 x 方向比 dst_w/cell 多 2 个点：
    #   header.mesh_width = dst_w/cell + 1
    #   data columns      = mesh_width + 1 = dst_w/cell + 2
    # y 方向保持 dst_h/cell + 1。少一列会导致板端 set_xylut 返回 0x80010000。
    grid_w = dst_w // cell_size + 2
    grid_h = dst_h // cell_size + 1

    # P_rect 取前 3×3 (= 矫正后的 K_new)
    P3 = np.array(P_rect, dtype=np.float64)
    if P3.shape == (3, 4):
        P3 = P3[:, :3]
    K_new = P3  # 矫正后相机内参

    # 预计算逆矩阵
    K_new_inv = np.linalg.inv(K_new)
    R_inv     = R_rect.T  # R 为正交矩阵, 逆 = 转置

    # 生成网格点坐标 (native space)
    gx_coords = np.arange(grid_w) * cell_size  # x includes one guard column past dst_w
    gy_coords = np.arange(grid_h) * cell_size

    # 网格点矩阵 (grid_h × grid_w)
    GX, GY = np.meshgrid(gx_coords, gy_coords)   # shape: (grid_h, grid_w)

    # ── 步骤1: native → calibrated ──
    X_cal, Y_cal = native_to_cal(GX, GY)

    # ── 步骤2: 归一化坐标 (K_new^-1 * [x, y, 1]^T) ──
    pts_cal = np.stack([X_cal, Y_cal, np.ones_like(X_cal)], axis=-1)  # (gh, gw, 3)
    pts_cal_flat = pts_cal.reshape(-1, 3).T  # (3, N)
    pts_norm = K_new_inv @ pts_cal_flat       # (3, N)  归一化坐标
    pts_norm = pts_norm[:2] / pts_norm[2:3]   # (2, N)

    # ── 步骤3: 逆矫正旋转 (R^T * [xn, yn, 1]^T) ──
    pts_norm3 = np.vstack([pts_norm, np.ones((1, pts_norm.shape[1]))])  # (3, N)
    pts_raw   = R_inv @ pts_norm3                                         # (3, N)
    pts_raw   = pts_raw[:2] / pts_raw[2:3]                               # (2, N)
    xr = pts_raw[0]  # (N,)
    yr = pts_raw[1]

    # ── 步骤4: 应用畸变 ──
    k1, k2, p1, p2, k3 = dist[0], dist[1], dist[2], dist[3], dist[4]
    r2 = xr**2 + yr**2
    r4 = r2 * r2
    r6 = r4 * r2
    if len(dist) >= 8:
        k4, k5, k6 = dist[5], dist[6], dist[7]
        radial = (1.0 + k1*r2 + k2*r4 + k3*r6) / (1.0 + k4*r2 + k5*r4 + k6*r6)
    else:
        radial = 1.0 + k1*r2 + k2*r4 + k3*r6
    # 切向畸变 (硬件 LDC 不支持, 但 LUT 可以完整包含)
    xd = xr * radial + 2*p1*xr*yr + p2*(r2 + 2*xr**2)
    yd = yr * radial + p1*(r2 + 2*yr**2) + 2*p2*xr*yr

    # ── 步骤5: 投影到原始图像坐标 (calibrated 1280×1080 空间) ──
    fx, fy = K_raw[0, 0], K_raw[1, 1]
    cx, cy = K_raw[0, 2], K_raw[1, 2]
    xs_cal = fx * xd + cx   # (N,) 在 calibrated space 中的 x 坐标
    ys_cal = fy * yd + cy   # (N,) 在 calibrated space 中的 y 坐标

    # ── 步骤6: calibrated → native ──
    xs_nat, ys_nat = cal_to_native(xs_cal, ys_cal)

    # reshape 回 (grid_h, grid_w)
    xs_nat = xs_nat.reshape(grid_h, grid_w).astype(np.float32)
    ys_nat = ys_nat.reshape(grid_h, grid_w).astype(np.float32)

    lut = np.stack([xs_nat, ys_nat], axis=-1)  # (grid_h, grid_w, 2)
    return lut, grid_w, grid_h, dst_w, dst_h


def write_lut_bin(lut, grid_w, grid_h, dst_w, dst_h, cell_size, out_path):
    """
    将 LUT 写入二进制文件 (ot_vproc_gdc_2dlut_head + LUT data)

    LUT data 格式: 每个网格点 2 × uint32 (Q28.4 fixed-point, coordinate × 16)
      src_x_q4 = (int)(src_x * 16)
      src_y_q4 = (int)(src_y * 16)
    行优先 (row-major): (gy=0,gx=0), (gy=0,gx=1), ..., (gy=grid_h-1, gx=grid_w-1)
    """
    # 写文件头 (100 bytes, #pragma pack(1))
    # 与官方样例 lut_2880X1344_map.bin header 格式一致:
    #   mesh_width  = grid_w - 1  (比 dst_w // cell_size 多 1 列保护网格)
    #   mesh_height = grid_h      (行点数, 即 dst_h // cell_size + 1)
    mesh_w  = grid_w - 1
    mesh_h  = grid_h
    header  = struct.pack('<6i', mesh_w, mesh_h, cell_size, cell_size, dst_w, dst_h)
    header += b'\x00' * 76         # reserved

    assert len(header) == LUT_HEADER_SIZE, f"header size={len(header)}"

    # 将 float 坐标转换为 Q28.4 定点 uint32。
    # 官方 sample LUT 的坐标按 /16 解释才落在像素坐标范围内；
    # 使用 Q16.16 会导致驱动采样超大坐标，VPROC 输出全黑。
    src_x = lut[:, :, 0]  # (grid_h, grid_w)
    src_y = lut[:, :, 1]

    # Q28.4: 像素坐标 * 16
    scale = 16.0
    # 允许轻微越界 (硬件做边界钳位)
    sx_q4 = np.round(src_x * scale).astype(np.int64)
    sy_q4 = np.round(src_y * scale).astype(np.int64)

    # 钳位到 uint32 范围 (0 ~ 2^32-1)
    sx_q4 = np.clip(sx_q4, 0, 0xFFFFFFFF).astype(np.uint32)
    sy_q4 = np.clip(sy_q4, 0, 0xFFFFFFFF).astype(np.uint32)

    # 交织为 (grid_h, grid_w, 2) uint32, row-major
    lut_u32 = np.stack([sx_q4, sy_q4], axis=-1)  # (grid_h, grid_w, 2)
    lut_bytes = lut_u32.tobytes()  # little-endian uint32 array

    with open(out_path, 'wb') as f:
        f.write(header)
        f.write(lut_bytes)

    total = LUT_HEADER_SIZE + len(lut_bytes)
    print(f"  写入: {out_path}  ({total/1024:.1f} KB, "
          f"grid={grid_w}×{grid_h}, cell={cell_size})")


def verify_lut(lut, K_raw, dist, R_rect, P_rect, cam_label, cell_size):
    """
    验证 LUT 精度:

    方法: 用 cv2.initUndistortRectifyMap 生成参考映射 (calibrated space),
    再将参考映射转换到 native space 与 LUT 逐点对比。

    LUT[gy,gx] = (src_x_native, src_y_native) : 输出网格点在 native space 中的采样坐标
    参考: map_ref[y_cal, x_cal] = (src_x_cal, src_y_cal) : OpenCV remap 在 cal space 的采样坐标

    验证步骤:
      1. 对每个网格点 (xd_nat, yd_nat):  转到 cal space → (xd_cal, yd_cal)
      2. 从参考 map 取 (src_x_cal_ref, src_y_cal_ref)
      3. 将 LUT 中的 (src_x_nat, src_y_nat) 转到 cal space → (src_x_cal_lut, src_y_cal_lut)
      4. 计算误差 = |(src_x_cal_lut, src_y_cal_lut) - (src_x_cal_ref, src_y_cal_ref)|
    """
    print(f"\n  [{cam_label}] LUT 精度验证...")

    P3 = np.array(P_rect, dtype=np.float64)
    if P3.shape == (3, 4):
        P3 = P3[:, :3]
    K_np  = np.array(K_raw, dtype=np.float64)
    dist_np = np.array(dist, dtype=np.float64)
    R_np  = np.array(R_rect, dtype=np.float64)

    # 生成 OpenCV 参考映射 (calibrated 1280×1080 空间)
    map_ref_x, map_ref_y = cv2.initUndistortRectifyMap(
        K_np, dist_np, R_np, P3,
        (CAL_W, CAL_H), cv2.CV_32FC1)
    # map_ref_x[y, x] = 原始图像中 x 坐标 (calibrated space)
    # map_ref_y[y, x] = 原始图像中 y 坐标 (calibrated space)

    grid_h, grid_w, _ = lut.shape
    errors = []

    # 对每个网格点采样
    for gy in range(grid_h):
        for gx in range(grid_w):
            xd_nat = gx * cell_size
            yd_nat = gy * cell_size
            if xd_nat > NATIVE_W or yd_nat > NATIVE_H:
                continue

            # 网格点 native → calibrated
            xd_cal, yd_cal = native_to_cal(xd_nat, yd_nat)

            # 边界检查
            xi = int(round(xd_cal))
            yi = int(round(yd_cal))
            if xi < 0 or xi >= CAL_W or yi < 0 or yi >= CAL_H:
                continue

            # 参考采样坐标 (calibrated space)
            ref_sx_cal = float(map_ref_x[yi, xi])
            ref_sy_cal = float(map_ref_y[yi, xi])

            # LUT 采样坐标 (native space) → calibrated space
            lut_sx_nat = float(lut[gy, gx, 0])
            lut_sy_nat = float(lut[gy, gx, 1])
            lut_sx_cal, lut_sy_cal = native_to_cal(lut_sx_nat, lut_sy_nat)

            err = np.sqrt((lut_sx_cal - ref_sx_cal)**2 + (lut_sy_cal - ref_sy_cal)**2)
            errors.append(err)

    if errors:
        mean_e = np.mean(errors)
        max_e  = np.max(errors)
        print(f"    采样点数: {len(errors)}")
        print(f"    误差: mean={mean_e:.4f}px, max={max_e:.4f}px")
        if mean_e < 0.1:
            print(f"    ✓ LUT 精度优秀 (mean<0.1px)")
        elif mean_e < 1.0:
            print(f"    ✓ LUT 精度良好 (mean<1.0px)")
        else:
            print(f"    ⚠ LUT 误差偏大, 请检查坐标变换")
        return mean_e
    return float('inf')


def verify_lut_visual(lut, K_raw, dist, R_rect, P_rect, cell_size,
                      out_prefix=None):
    """
    可视化验证: 生成 undistort+rectify 映射图像并保存。
    创建一张标准网格测试图, 用 LUT 做 remap, 观察网格直线度。
    """
    P3 = np.array(P_rect, dtype=np.float64)
    if P3.shape == (3, 4):
        P3 = P3[:, :3]

    K_raw_np = np.array(K_raw, dtype=np.float64)

    # 生成 opencv remap maps (用于对比)
    map1, map2 = cv2.initUndistortRectifyMap(
        K_raw_np, np.array(dist, dtype=np.float64),
        np.array(R_rect, dtype=np.float64), P3,
        (CAL_W, CAL_H), cv2.CV_32FC1)

    # 从 LUT 反推出 remap map (calibrated space → calibrated space)
    grid_h, grid_w, _ = lut.shape

    # 先将 LUT (native space) 转换到 calibrated space
    # LUT: output_native → source_native
    # remap: output_cal → source_cal
    lut_src_x_cal = np.zeros((grid_h, grid_w), dtype=np.float32)
    lut_src_y_cal = np.zeros((grid_h, grid_w), dtype=np.float32)
    for gy in range(grid_h):
        for gx in range(grid_w):
            sx_nat = lut[gy, gx, 0]
            sy_nat = lut[gy, gx, 1]
            sx_cal, sy_cal = native_to_cal(sx_nat, sy_nat)
            lut_src_x_cal[gy, gx] = sx_cal
            lut_src_y_cal[gy, gx] = sy_cal

    # 构造一张测试网格图 (calibrated space 1280×1080)
    test_img = np.zeros((CAL_H, CAL_W, 3), dtype=np.uint8)
    for y in range(0, CAL_H, 32):
        cv2.line(test_img, (0, y), (CAL_W, y), (0, 180, 0), 1)
    for x in range(0, CAL_W, 32):
        cv2.line(test_img, (x, 0), (x, CAL_H), (0, 180, 0), 1)
    cv2.putText(test_img, "Grid test (calibrated space)",
                (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 255, 255), 2)

    # OpenCV remap 参考结果
    ref_result = cv2.remap(test_img, map1, map2, cv2.INTER_LINEAR)

    if out_prefix:
        cv2.imwrite(f"{out_prefix}_opencv_remap.jpg", ref_result)
        print(f"    参考图: {out_prefix}_opencv_remap.jpg")

    return ref_result


# ── 主程序 ────────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(
        description='生成 SD3589C VPROC GDC 2D-LUT 文件 (undistort + rectify)')
    parser.add_argument('--calib', default=os.path.join(os.path.dirname(__file__),
                        'stereo_calib.json'), help='标定文件路径')
    parser.add_argument('--cell', type=int, default=16,
                        choices=[16, 32, 64, 128, 256], help='网格单元大小 (像素)')
    parser.add_argument('--out-left',  default='lut_left.bin',  help='左相机 LUT 输出文件')
    parser.add_argument('--out-right', default='lut_right.bin', help='右相机 LUT 输出文件')
    parser.add_argument('--verify', action='store_true', help='验证 LUT 精度')
    parser.add_argument('--visual', action='store_true', help='生成可视化对比图')
    args = parser.parse_args()

    # 加载标定文件
    print(f"[generate_xylut] 加载标定: {args.calib}")
    with open(args.calib, 'r') as f:
        calib = json.load(f)

    img_size = calib.get('image_size', [CAL_W, CAL_H])
    assert img_size[0] == CAL_W and img_size[1] == CAL_H, \
        f"仅支持 1280×1080 标定图像, 当前: {img_size}"

    rect = calib['rectification']
    R1 = np.array(rect['R1'], dtype=np.float64)
    R2 = np.array(rect['R2'], dtype=np.float64)
    P1 = np.array(rect['P1'], dtype=np.float64)
    P2 = np.array(rect['P2'], dtype=np.float64)

    def parse_cam(obj):
        K = np.array([[obj['fx'], 0, obj['cx']],
                      [0, obj['fy'], obj['cy']],
                      [0, 0, 1]], dtype=np.float64)
        dist = np.array(obj['distortion'], dtype=np.float64)
        return K, dist

    K_L, dist_L = parse_cam(calib['left'])
    K_R, dist_R = parse_cam(calib['right'])

    print(f"[generate_xylut] 左相机: fx={K_L[0,0]:.1f} fy={K_L[1,1]:.1f} "
          f"cx={K_L[0,2]:.1f} cy={K_L[1,2]:.1f}")
    print(f"[generate_xylut] 右相机: fx={K_R[0,0]:.1f} fy={K_R[1,1]:.1f} "
          f"cx={K_R[0,2]:.1f} cy={K_R[1,2]:.1f}")
    print(f"[generate_xylut] cell_size={args.cell}, "
          f"native={NATIVE_W}×{NATIVE_H}, calibrated={CAL_W}×{CAL_H}")
    print()

    # ── 生成左相机 LUT ──
    print("=== 左相机 LUT ===")
    lut_L, gw_L, gh_L, dw_L, dh_L = compute_lut(K_L, dist_L, R1, P1, args.cell)
    write_lut_bin(lut_L, gw_L, gh_L, dw_L, dh_L, args.cell, args.out_left)
    if args.verify:
        verify_lut(lut_L, K_L, dist_L, R1, P1, "Left", args.cell)
    if args.visual:
        out_pfx = args.out_left.replace('.bin', '')
        verify_lut_visual(lut_L, K_L, dist_L, R1, P1, args.cell, out_pfx)

    # ── 生成右相机 LUT ──
    print("\n=== 右相机 LUT ===")
    lut_R, gw_R, gh_R, dw_R, dh_R = compute_lut(K_R, dist_R, R2, P2, args.cell)
    write_lut_bin(lut_R, gw_R, gh_R, dw_R, dh_R, args.cell, args.out_right)
    if args.verify:
        verify_lut(lut_R, K_R, dist_R, R2, P2, "Right", args.cell)
    if args.visual:
        out_pfx = args.out_right.replace('.bin', '')
        verify_lut_visual(lut_R, K_R, dist_R, R2, P2, args.cell, out_pfx)

    print("\n[generate_xylut] 完成。")
    print(f"  LUT 文件已生成: {args.out_left}, {args.out_right}")
    print(f"  将两个 .bin 文件部署到板端: /opt/stereo/lut_left.bin, /opt/stereo/lut_right.bin")


if __name__ == '__main__':
    main()
