---
name: sd3589c-stereo-dev
description: SD3589C平台stereo_app双目立体视觉应用开发指南。涵盖VI/VPROC/XY-LUT/CVE/NPU/SubPixel/VENC/网络传输、raw-only原始图像采集、scene_auto、receiver预览保存等关键经验、接口使用、参数约束和已知坑点。当开发stereo_app新功能、排查硬件模块错误、优化流水线性能、采集原始图像、构建部署或板端运行验证时使用。
---

# SD3589C stereo_app 开发指南

## 系统架构

```
VI(SC132GS 1080×1280 @20fps × 2, DIRECT_SLAVE FSIN触发)
  → VPROC (GDC/XY-LUT去畸变+极线矫正 + 全幅Crop/Rotation → 1280×1080)
    → CVE (下采样640×540 + Crop640×448 + CSC RGB888 planar + identity map)
      → NPU (dual-output stereo_match.ortm → cost[96,224,320] + disp[224,320])
        → SubPixel (5-tap parabolic → uint16 Q5 320×224)
          → 2×最近邻上采样 → uint16 Q5 640×448
            → VENC (JPEG q=75 left/right + LZ4 disparity 640×448)
              → Network (TCP, type=6 calib + type=7 perf)
```

### 运行时日志已禁用

**背景**: 运行时日志（stdout printf + fflush + 文件 fprintf + mutex）在每帧流水线中引入不必要的 I/O 开销和 mutex 竞争，尤其在高帧率流水线中影响性能。

**实现**: 将两个日志函数改为空函数体：
- `stereo_log_write()` (`stereo_main.c`) — 核心日志函数，所有模块通过 `STEREO_LOG()` 宏和直接调用。改为空函数，去除 stdout printf/fflush、文件 fopen/fprintf/fflush、mutex 加解锁。
- `stereo_network_write_log()` (`stereo_network.c`) — 每 20 帧的 FPS 统计日志。改为空函数。

**保留**: PERF_START/PERF_END 宏不涉及 I/O，仅将耗时存入全局变量 `g_perf_npu_ms`/`g_perf_sub_ms`，通过网络 JSON 帧传输给上位机显示。

**验证**: `STEREO_LOG`/FPS 统计不再输出；但 stdout 仍有不受影响的第三方输出：`media deinit success!`、`vrb config success!`、`media init success!`（sample_comm_sys 的 printf）、NPU 运行时的 `model verification successful`/`LoadFromMem` 等 Debug 行，以及非致命的 `crypto_ioctl failed 0x13200007`。因此模块8 的“成功日志标志”依然可用于启动验证。

### CVE identity map 双缓冲（uint8 RGB888 直传 + 闪烁修复）

**背景**: 早期实现中，CVE CSC 输出后需用 XOR 0x80 LUT 将 uint8 转为 int8（RGB-128），因为 NPU 硬件直接 reinterpret 字节为 int8，不执行 zero_point 减法。**当前已将减128预处理迁移至 ORTM 模型的 DTC Preprocess 节点**，CVE 输出原始 uint8 RGB888 planar 数据即可，map 操作改用 identity LUT（`lut[i] = i`）。

**实现**: 在 CVE CSC 输出后，使用 `ot_avp_cve_map` 算子对 U8C3_PLANAR 的 3 个平面分别应用 256 项 identity LUT（`lut[i] = i`），将 CSC buffer 数据拷贝到独立的 NPU 输入 buffer。map 操作本身不改变像素值，仅利用 CVE 硬件完成 buffer 间拷贝并保持双缓冲架构。

**闪烁根因**: 初版实现中 map 操作就地写入 CSC 输出 buffer（`g_cve_rgb_*`），而该 buffer 同时作为 NPU 输入通过队列传递。当 CVE 线程处理下一帧时，CSC 先写入新数据覆盖旧数据，若 NPU 尚未读完，就会读到不完整数据，产生约 1/30 概率的闪烁帧。

**修复方案 — 双缓冲 + 队列深度降为 1**:
1. 分配 2 套独立 NPU 输入 buffer（`g_cve_npu_left[2]`、`g_cve_npu_right[2]`），map 从 CSC buffer 写入到 NPU buffer
2. CVE 和 NPU 交替使用不同 buffer（`g_cve_buf_idx` 乒乓切换），永远不会同时访问同一 buffer
3. NPU 输入队列深度从 8 降为 1，确保 CVE 完成一帧后 NPU 才开始处理

**关键代码** (`stereo_cve.c`):
```c
#define STEREO_CVE_NPU_BUF_NUM  2
static ot_avp_cve_img g_cve_npu_left[STEREO_CVE_NPU_BUF_NUM];
static ot_avp_cve_img g_cve_npu_right[STEREO_CVE_NPU_BUF_NUM];
static ot_u32 g_cve_buf_idx = 0;

// stereo_cve_process() Step 4: identity map to double-buffered NPU input
ot_u32 buf_idx = g_cve_buf_idx;
stereo_cve_xor_sign_bit(&g_cve_rgb_left, &g_cve_npu_left[buf_idx], &g_map_handle_l);
stereo_cve_xor_sign_bit(&g_cve_rgb_right, &g_cve_npu_right[buf_idx], &g_map_handle_r);
*left_crop  = g_cve_npu_left[buf_idx];
*right_crop = g_cve_npu_right[buf_idx];
g_cve_buf_idx = (g_cve_buf_idx + 1) % STEREO_CVE_NPU_BUF_NUM;
```

**stereo_cve_xor_sign_bit()** 核心逻辑:
```c
// LUT is pre-filled in init: lut[i] = i (identity, no XOR)
// DTC Preprocess node handles subtract-128 internally
for (ot_s32 ch = 0; ch < 3; ch++) {
    src_plane.virt_addr[0] = src_img->virt_addr[ch];
    src_plane.phys_addr[0] = src_img->phys_addr[ch];
    dst_plane.virt_addr[0] = dst_img->virt_addr[ch];
    dst_plane.phys_addr[0] = dst_img->phys_addr[ch];
    ot_avp_cve_map(handle, &src_plane, &lut_data, &dst_plane, OT_AVP_CVE_INVOKE_SYNC);
}
```

**NPU 队列深度** (`stereo_media.c`): `STEREO_NPU_QUEUE_DEPTH` 从 8 改为 1。

**验证方法**: 用 `capture_disp.py` 连续捕获 60+ 帧视差，检查是否仍有异常帧（max disparity >1024 或 mean 突变）。修复后无闪烁帧。

### 当前完整链路流水线
```
get_frame → g_q_vproc → cve → g_q_npu → npu(+SubPixel串行) → 2×上采样 → g_q_venc → venc → g_q_net → net
```
当前项目**只支持 1280×1080 标定方案**：VI 原生帧为 1080×1280，VPROC/GDC/XY-LUT 在原生全帧上处理，随后全幅 crop + rot270 输出 1280×1080；所有标定、LUT、VPROC、CVE、VENC 与部署路径均以该尺寸为唯一基准。

### 低延迟（glass-to-glass）优化

**背景**: 原始链路存在数帧隐藏延迟，主要来自 VI 3DNR 参考帧缓存、VPROC 通道 FIFO 缓冲、各级 pipeline 队列的陈旧帧堆积。以下为已落地的核心低延迟优化（延迟调试期的细粒度 per-stage timing 测量代码已全部移除，仅保留这些核心优化和原有 npu/sub perf 透传）。

**1. 禁用 VI 时域 3DNR**（`stereo_media.c`，VI 启动前）:
```c
/* Disable temporal 3DNR to eliminate 1-frame reference buffer latency.
   For stereo matching, the NPU model tolerates sensor noise; saves ~33ms. */
ctx->vi_cfg[i].pipe_info[0].nr_attr.enable = OT_FALSE;
```
时域 3DNR 需缓存 1 帧参考帧，直接引入约 33ms（1 帧 @20fps）延迟。双目匹配对传感器噪声不敏感，禁用后视差质量无明显下降。

**2. 队列深度收敛**:
- `stereo_types.h`: `STEREO_QUEUE_CAPACITY 8→2`（SPSC 环形队列，须为 2 的幂）、`STEREO_VPROC_QUEUE_DEPTH 6→2`
- `stereo_media.c` 队列 init：`g_q_vproc`、`g_q_npu` 深度设为 **1**（vproc depth 1 + drop-if-full 使 vproc_q 延迟最小；npu depth 1 保证 CVE 双缓冲安全，不覆盖 NPU 正在读的输入 buffer）
```c
stereo_queue_init(&g_q_vproc, 1);  /* Depth 1 + drop-if-full = minimal vproc_q latency */
stereo_queue_init(&g_q_npu,   1);  /* CVE double-buffer safety */
stereo_queue_init(&g_q_venc,  STEREO_QUEUE_CAPACITY);
stereo_queue_init(&g_q_net,   STEREO_QUEUE_CAPACITY);
```

**3. drain-to-latest（get_frame 排空到最新同步帧对）**: VPROC 通道 FIFO 缓冲多帧，acquire 取到的是最旧帧。get_frame 每轮成对 drain 左右通道到最新同步帧对，丢弃旧帧，消除时间戳打点前的隐藏延迟。**必须 L/R lockstep 排空**以保持双目同步：
```c
ot_eis_img_frame nl, nr;
while (ot_eis_vproc_chn_acquire_frame(chn_l, &nl, 0) == OT_SUCCESS) {
    if (ot_eis_vproc_chn_acquire_frame(chn_r, &nr, 0) == OT_SUCCESS) {
        /* 有更新的同步帧对 → 丢弃旧对 */
        ot_eis_vproc_chn_release_frame(chn_l, &pair->left_full);
        ot_eis_vproc_chn_release_frame(chn_r, &pair->right_full);
        pair->left_full = nl;  pair->right_full = nr;
    } else {
        /* 有新 L 但 R 尚未到 → 取新 L，保留旧 R，停止 */
        ot_eis_vproc_chn_release_frame(chn_l, &pair->left_full);
        pair->left_full = nl;  break;
    }
}
```

**4. drop-if-full 丢帧机制**: 各阶段（get_frame/CVE/NPU/VENC）用 `stereo_queue_try_push()` 入队，队列满则**立即丢弃当前帧**并释放其资源（VPROC 帧 release 回硬件池、malloc buffer free），而非阻塞重试。防止陈旧帧累积，保证下游始终处理最新帧。切勿在 push 处 busy-loop 重试，否则会与已排空的上游产生死锁风险。

**5. glass-to-glass 延迟盲区**: `board_e2e` 时间戳打点在 VPROC 输出（get_frame）之后，**无法反映 sensor 曝光 / VI / ISP / VPROC GDC 硬件固有延迟**（含 3DNR 的 33ms）。评估真实 glass-to-glass 延迟需外部手段（如拍摄屏幕计时器与实时预览的同框差）。drain-to-latest 消除的正是 board_e2e 打点**之前**的这段隐藏延迟。

### 丢帧环节分析

流水线为多线程流水线结构（非完全串行），各线程通过 SPSC 队列连接并行处理不同帧。但队列深度极小，限制了并行度：

```
Sensor(20fps,50ms) → VPROC(depth=2) → [get_frame] → g_q_vproc(深度1) → [cve_proc] → g_q_npu(深度1) → [npu_proc] → g_q_venc(深度2) → [venc_proc] → g_q_net(深度2) → [net_proc] → TCP
```

6 个丢帧环节（按概率从高到低）：
1. **g_q_npu 满溢**（最可能）— NPU ~47ms vs 传感器 50ms 周期，余量仅 3ms，NPU 抖动即丢帧
2. **g_q_vproc 满溢** — CVE+下游慢时 get_frame push 失败
3. **get_frame drain 丢弃**（设计性）— VPROC 队列积压时主动丢弃旧帧保持低延迟
4. **g_q_net 满溢**（TCP 背压）— 上位机接收慢，net_proc send 阻塞
5. **无客户端时丢弃**（设计性）— net_proc 主动清空队列防止积压
6. **VPROC 硬件级队列溢出** — get_frame 阻塞时硬件层丢帧，软件不可见

线程级并行：NPU 处理 frame N 时，get_frame+CVE 处理 N+1，VENC+NET 处理 N-1。实际吞吐由最慢阶段（NPU ~47ms）决定，而非所有阶段之和（~58ms）。

### 板端视差2×最近邻上采样（320×224 → 640×448）

**背景**: NPU 原生输出 320×224 视差，上位机显示和测距需要更高分辨率。在 NPU 线程内 SubPixel 完成后，对 uint16 Q5 视差做 2× 最近邻上采样至 640×448，再推入 VENC 队列。

**实现** (`stereo_media.c` `stereo_npu_proc()`):
```c
ot_u16 *upsampled = (ot_u16 *)malloc(up_w * up_h * sizeof(ot_u16));
for (ot_u32 y = 0; y < up_h; y++) {
    ot_u32 sy = y / 2;
    if (sy >= sub_h) sy = sub_h - 1;
    const ot_u16 *src_row = out->disparity + sy * sub_w;
    ot_u16 *dst_row = upsampled + y * up_w;
    for (ot_u32 x = 0; x < up_w; x++) {
        ot_u32 sx = x / 2;
        if (sx >= sub_w) sx = sub_w - 1;
        dst_row[x] = src_row[sx];
    }
}
free(out->disparity);
out->disparity = upsampled;
out->disp_bytes = up_w * up_h * sizeof(ot_u16);  // 614400 bytes
```

**标定 JSON 适配** (`stereo_network.c`):
- `scale_x` 从 4.0 改为 2.0（1280/640）
- `disp_scale` 从 2.0 改为 1.0（上采样后传输分辨率 = NPU 输入分辨率）
- `disp_w`/`disp_h` 更新为 640/448

**关键约束**: 上采样不改变视差值（仍为 NPU 输入分辨率 640 空间的值），仅重复像素。测距公式中 `disp_scale=1.0` 表示无需额外缩放。

当前去除 NPU/SubPixel 双缓冲：`stereo_npu.c` 仅保留 1 组 NPU 输出 buffer，`stereo_npu_release_buf_set()` 为兼容 no-op；SubPixel 在 NPU 线程内同步处理，处理完成后才进入下一次 NPU 推理。

## 关键路径配置

| 项目 | 路径 |
|------|------|
| 板端IP | 192.168.1.101, SSH密码: root（新设备）或 123456（旧设备） |
| 可执行文件 | `/opt/stereo/stereo_app` |
| 标定文件 | `/opt/stereo/stereo_calib.json` |
| 原始模型 | `/data/model/stereo_match.ortm`（未加密，已弃用） |
| 加密模型 | `/data/model/stereo_match.ortm.enc`（当前使用） |
| license文件 | `/opt/stereo/license.bin`（64B，设备绑定授权） |
| auth_gen | `/tmp/auth_gen`（临时部署，运行后可删除） |
| scene参数 | `/opt/stereo/param/sc132gs/` |
| yolo模型 | `/opt/model/tiny-yolov3_yuv420sp_b.ortm`（9MB，`/data` 放不下，放 `/opt`） |
| 源码 | `main/src/component/media/sample/stereo_app_bk/`（当前仓库布局，SDK 根=仓库根） |
| 模型源文件 | 仓库根 `stereo_s_ori_h448_w640_128_sub_v1.7_e300_sim.ortm`（明文，加密后部署） |
| 交叉工具链 | `/opt/linux/x86-arm/aarch64-otv02-linux-gnu-gcc/bin`（备选 `/home/lzx/gcc-aarch64-otv02-linux-gnu/aarch64-otv02-linux-gnu-gcc/bin`） |

## 构建与部署

```bash
# 当前仓库没有 build_stereo_app.sh，直接用本机交叉工具链编译：
cd /home/lzx/lq560v100_sdk
export PATH=/opt/linux/x86-arm/aarch64-otv02-linux-gnu-gcc/bin:$PATH
SDK=/home/lzx/lq560v100_sdk

# 注意顺序：stereo 的 clean 会连带删除 common 的 .o，所以先 clean stereo 再 build common
make -C main/src/component/media/sample/stereo_app_bk clean
make -C main/src/component/media/sample/common clean
make -C main/src/component/media/sample/common COMMON_DIR=$SDK/main/src/component/media/sample/common
# 用 stereo_app 目标（而非 all/install）避免把产物拷进 userfs
make -C main/src/component/media/sample/stereo_app_bk COMMON_DIR=$SDK/main/src/component/media/sample/common stereo_app
make -C main/src/component/media/sample/stereo_app_bk COMMON_DIR=$SDK/main/src/component/media/sample/common auth_gen

# 手动部署二进制、1280×1080 标定和 LUT：
sshpass -p "123456" scp main/src/component/media/sample/stereo_app_bk/stereo_app root@192.168.1.101:/opt/stereo/stereo_app
sshpass -p "123456" scp main/src/component/media/sample/stereo_app_bk/scripts/stereo_calib.json root@192.168.1.101:/opt/stereo/stereo_calib.json
sshpass -p "123456" scp main/src/component/media/sample/stereo_app_bk/scripts/lut_left.bin root@192.168.1.101:/opt/stereo/lut_left.bin
sshpass -p "123456" scp main/src/component/media/sample/stereo_app_bk/scripts/lut_right.bin root@192.168.1.101:/opt/stereo/lut_right.bin
# 模型必须按“模块8”用板端 UID 加密，再部署为 /data/model/stereo_match.ortm.enc
# yolo 模型（非设备绑定，直接拷贝）：
sshpass -p "123456" scp ~/npu_toolchain/common/samples/tiny-yolov3_yuv420sp/tiny-yolov3_yuv420sp_b.ortm root@192.168.1.101:/opt/model/

# 运行完整链路
sshpass -p "123456" ssh root@192.168.1.101 "cd /opt/stereo && ./stereo_app"

# 运行raw-only原始图像采集链路
sshpass -p "123456" ssh root@192.168.1.101 "cd /opt/stereo && ./stereo_app --raw-only"
```

**板端存储**: 加密模型约 27.5MB，`/data` 是 UBIFS 且总容量只有约 35MB。空间不足时删除 `/data/npu_demo` 下旧的 abab demo 模型（mobilenetv2/tiny-yolov3）、bin 与 lib 中的构建产物（均可从 SDK 重编），用 `df -k /data` 确认余量后再上传模型。

**Makefile 无头文件依赖**: stereo_app 的 Makefile 没有 `-MMD` 依赖，只改 `.h` 不会重编对应 `.o`（例如只改 `stereo_yolo.h` 里的模型路径，二进制不会更新）。改头文件后必须 `touch` 对应 `.c` 或走完整 clean 流程，部署后可用 `strings stereo_app | grep 路径` 复核。

**停止进程的正确姿势**: busybox 的 `pgrep -x`/`pkill -x` 不匹配 comm，`pkill -f stereo_app` 还会误杀远程 SSH 的包装 shell。用 `/proc` comm 扫描按名找 PID 再 `kill -INT`（勿 kill -9）：

```sh
ssh root@192.168.1.101 'kill -INT $(for p in /proc/[0-9]*; do c=$(cat $p/comm 2>/dev/null); [ "$c" = "stereo_app" ] && echo ${p#/proc/}; done)'
```

**坑**: 完整链路每次测试前建议 `reboot` 板子。若 VENC 通道残留（kill -9 后未正常 deinit），`ot_eis_venc_chn_create()` 会返回 `0x80080052`（VENC_EXIST）。当前代码已增加自动恢复机制：检测到 chn0 创建失败时，自动执行 `ot_eis_venc_exit()` + `ot_eis_venc_init()` + 重试创建。raw-only 不启动 VENC，可用于绕开 VENC 资源冲突并快速验证 VI/ISP/raw capture。

### 构建脚本精简部署（SCP排除标定与LUT）

`build_stereo_app.sh` 仅部署编译后的 `stereo_app` 二进制到板端 `/opt/stereo/`，不再自动部署标定文件（`stereo_calib.json`）和 LUT 文件（`lut_left.bin`/`lut_right.bin`）。标定和 LUT 需手动部署，避免构建脚本覆盖已有标定结果。

### 性能计时透传（全局变量 + 网络发送）

性能耗时不打印到 stdout，改为存储到全局变量，每帧通过 type=7 JSON 帧发送给上位机显示。

**板端实现** (`stereo_media.c`):
```c
static volatile double g_perf_npu_ms = 0;
static volatile double g_perf_sub_ms = 0;

#define PERF_START(tag) \
    struct timeval _pf_##tag##_s; gettimeofday(&_pf_##tag##_s, NULL)
#define PERF_END(tag, var) do { \
    struct timeval _pf_##tag##_e; gettimeofday(&_pf_##tag##_e, NULL); \
    g_perf_##var = (tv差值转ms); \
} while(0)

// 调用处：
// PERF_END(npu, npu_ms);      → g_perf_npu_ms
// PERF_END(subpix, sub_ms);   → g_perf_sub_ms

void stereo_media_get_perf(double *npu_ms, double *sub_ms);
```

**网络发送** (`stereo_network.c`，每帧三元组后发送):
```c
double npu_ms, sub_ms;
stereo_media_get_perf(&npu_ms, &sub_ms);
// JSON: {"npu_ms":49.6,"sub_ms":8.9,"total_ms":58.4}
perf_pkt.type = STEREO_FRAME_TYPE_PERF;  // 0x07
stereo_network_send_packet(g_client_fd, &perf_pkt);
```

**上位机** (`stereo_receiver.py`): 接收 type=7 帧，解析 JSON，显示在信息面板 NPU/SubPixel/Total 行。

---

## 模块0: raw-only 原始图像采集

### 功能定位
`./stereo_app --raw-only` 是最小化原始图像采集链路：仅启动双路 VI、`scene_auto` ISP tuning 和 9001 raw capture 服务；不启动 VPROC/CVE/NPU/SubPixel/VENC/9000 实时流。

### 核心文件
- `stereo_main.c` — 解析 `--raw-only`/`--help`
- `stereo_media.c/h` — `stereo_media_set_raw_only()`，raw-only init/startup/shutdown 分支
- `stereo_raw_capture.c` — 9001 raw capture 服务，VI dump、mmap、RAW0封包、发送
- `stereo_scene_auto.c/h` — scene_auto 启停保护，避免未启动时重复 deinit
- `receiver/stereo_receiver.py` — `--capture-raw`、`--show-raw`、实时预览与按键保存

### 运行方式
```bash
# 板端
cd /opt/stereo
./stereo_app --raw-only

# 上位机：一次采集并立即保存
python3 stereo_receiver.py --host 192.168.1.101 --capture-raw

# 上位机：实时预览，按 s 保存当前左右图，按 q 退出
python3 stereo_receiver.py --host 192.168.1.101 --capture-raw --show-raw
```

### raw capture 协议
- 控制端口：TCP `9001`，独立于实时流 `9000`
- 上位机命令：`struct.pack('>IBB2s', 0x53545243, 0x01, mask, b'\x00\x00')`，magic 为 `STRC`
- 数据包头：24字节 `HEADER_FMT = '>IBIQI3s'`，magic 为 `STMP`
- raw frame type：`0x04=raw_left`，`0x05=raw_right`
- RAW0 metadata：24字节 `RAW_META_FMT = '>4sHHHHBBHII'`

### 数据保真要求
- `.nv21`：直接写板端传来的 raw payload，不经过 GUI 缩放、JPEG 压缩或 OpenCV resize；原始尺寸通常为 `1080x1280_nv21`。
- `.png`：由同一份 NV21 原始数据转 BGR 后以 PNG 无损压缩保存；旋转后常见尺寸为 `1280x1080.png`。
- GUI 预览：每路显示宽度为 640，高度按比例缩放；预览缩放不影响保存文件。

### 板端实现坑点
- `ot_eis_vi_chn_get_frame()` 返回的 `virt_addr[0]` 可能为空，不能直接 memcpy；必须用 `phys_addr[0/1]` + `ot_omi_sys_mmap()` 映射 Y/UV plane，再 `ot_omi_sys_munmap()`。
- raw payload 约 2MB，client fd 需清除 `O_NONBLOCK`；`raw_send_all()` 必须处理 `EINTR/EAGAIN/EWOULDBLOCK` 并循环发送。
- Python RAW meta 必须按 24 字节解析；旧 20 字节格式会导致 `raw data size mismatch`。

---

## 模块1: VPROC XY-LUT 双目极线矫正

### 功能定位
当前 `stereo_app` **只保留 XY-LUT 硬件矫正路径**：在 VPROC GDC 阶段用二维查找表一次性完成 **8参数 rational 去畸变 + stereoRectify 极线矫正**，随后保持 `grp_set_crop(1080×1280)` 和 `Rotation270°` 输出 `1280×1080`。

历史非 LUT 矫正路径已从 `stereo_rectify.h/c` 和 `stereo_media.c` 删除，不再作为 fallback 使用；`stereo_app` 启动时若左右 LUT 加载失败，应直接中止完整链路启动。

### 核心文件
- `stereo_rectify.h/c` — `/opt/stereo/lut_left.bin`、`/opt/stereo/lut_right.bin` 加载，`set_gdc + set_xylut` 应用，释放 LUT SMR 资源
- `stereo_media.c` — VPROC group start 后、VI bind 前强制应用 XY-LUT，避免未矫正帧进入流水线
- `scripts/generate_xylut.py` — 基于最新标定 JSON 生成左右 LUT
- `scripts/verify_rectify.py` — 对板端输出图直接做角点检测和 Y-disparity 验证
- `sample/common/sample_comm_vproc.c` — `sample_comm_vproc_load_gdc_2dlut_file()` 读取 LUT header 并分配 SMR

### 正确调用序列
1. 启动 VI，初始化 VPROC attr 与用户 buffer pool
2. `stereo_rectify_load_xylut(1080, 1280)` 预加载左右 LUT，构建 `ot_eis_vproc_gdc_cfg` 和 `ot_eis_vproc_xylut_param`
3. 按 LUT header 中 `dst=1088×1280` 将 VPROC `grp_attr.max_width` 更新为 1088，避免 GDC 输出宽度超过 group max
4. `sample_comm_start_vproc()` 启动左右 VPROC group
5. **VI bind 前**调用 `stereo_rectify_apply_xylut(left_grp, right_grp)`：每路依次 `ot_eis_vproc_grp_set_gdc()`、`ot_eis_vproc_grp_set_xylut()`
6. 再执行 VI→VPROC bind、scene_auto、raw_capture、线程启动

### LUT 工作坐标系
- GDC 输入/输出工作在传感器原生空间：`1080×1280`
- 标定 JSON 的 `image_size=[1280,1080]` 是 Rotation270° 后的坐标系
- native → calibrated：`x_cal = y_native`，`y_cal = 1079 - x_native`
- calibrated → native：`x_native = 1079 - y_cal`，`y_native = x_cal`
- 生成 LUT 时对每个 native 输出网格点反投影到 rectified calibration space，再经 `R^-1`、rational distortion、原始 K 投影回原始采样坐标，最后转回 native 采样点

### LUT 文件格式关键约束
- Header 为 100 bytes：`<6i + reserved[76]`
- 对 1080×1280 native 帧，GDC 输出宽度需 16 对齐到 `dst_width=1088`，高度 `dst_height=1280`
- **X 方向必须多 1 列保护网格**：`grid_w = dst_w // cell + 2`，`mesh_width = grid_w - 1`
- Y 方向：`mesh_height = dst_h // cell + 1`
- 数据长度：`grid_w × mesh_height × 8` 字节，每个点为两个 `uint32` 坐标
- 1088×1280、cell=16 示例：header `(69, 81, 16, 16, 1088, 1280)`，数据长 `70×81×8=45360` 字节，文件总长 45460 bytes
- **坐标定点格式必须为像素坐标×16(Q28.4)**，不是 Q16.16。误用 `×65536` 时 `set_xylut` 可能返回 OK，但 VPROC 会采样超大坐标并输出全黑。

### 生成与部署
```bash
cd main/src/component/media/sample/stereo_app_bk/scripts
python3 generate_xylut.py \
  --calib captured/calib_result/stereo_calib.json \
  --out-left lut_left.bin \
  --out-right lut_right.bin \
  --verify

sshpass -p "123456" scp lut_left.bin root@192.168.1.101:/opt/stereo/lut_left.bin
sshpass -p "123456" scp lut_right.bin root@192.168.1.101:/opt/stereo/lut_right.bin
```

### 成功日志
板端启动应出现：
```
[stereo_rectify] left XY-LUT loaded: src=1080x1280 dst=1088x1280 ... len=45360
[stereo_rectify] right XY-LUT loaded: src=1080x1280 dst=1088x1280 ... len=45360
[stereo_rectify] left set_gdc(XY-LUT) OK
[stereo_rectify] left set_xylut OK
[stereo_rectify] right set_gdc(XY-LUT) OK
[stereo_rectify] right set_xylut OK
[stereo_rectify] XY-LUT stereo rectification applied
```

### 常见问题
| 现象 | 根因 | 处理 |
|------|------|------|
| `load XY-LUT failed` | `/opt/stereo/lut_left.bin` 或 `/opt/stereo/lut_right.bin` 缺失/损坏 | 重新运行 `generate_xylut.py` 并部署左右 LUT |
| `set_xylut` 返回 `0x80010000`，驱动打印 `set lut to vpp failed` | LUT X 方向少保护列，header/data 长度与官方格式不一致 | 使用 `grid_w=dst_w//cell+2`、`mesh_width=grid_w-1` 重新生成 |
| `set_xylut OK` 但左右图全黑 | LUT 坐标误写成 `×65536(Q16.16)`，驱动按 `×16` 解释后采样坐标溢出 | 坐标按 `round(coord * 16)` 写入 uint32 |
| 重新部署 LUT 后仍黑图 | 板端旧 `stereo_app` 进程仍在使用旧 LUT | `pgrep stereo_app`，用 `kill -INT <pid>` 正常退出后重启 |
| 验证脚本检测不到棋盘格 | 实拍靶标检测规格与脚本不一致，或传统检测器不鲁棒 | 当前实拍靶标按 `9×6` 内角点检测，并用 `findChessboardCornersSB` 兜底 |
| Y-disparity 异常偏大 | 对已经由板端 XY-LUT 矫正过的图又做了一次 OpenCV rectify | 验证时直接在板端输出图上检测左右同名角点并计算 `y_left-y_right` |

### 验证基线
- 使用当前实拍靶标：约 30cm，25mm 方格，按 `9×6` 内角点检测
- `verify_rectify.py` 对板端输出图直接测 Y-disparity，不再二次 OpenCV remap
- 已验证一组样例：左/右角点行直线度约 `0.12px`，`mean|dy|≈1.0px`，`max|dy|≈2.5px`

---

## 模块2: NPU 推理

### 核心文件
- `stereo_npu.c` — 模型加载、单输出buffer set串行推理执行
- `stereo_npu.h` — 接口定义 (`stereo_npu_infer` 保留 `buf_set_idx` 兼容输出，当前始终为0)
- 参考: `sample/media_ai/ot_avp_cve_alg_lib.c`

### 双输出模型
- Input 0: left 640×448 RGB888 planar (uint8, DTC Preprocess handles -128)
- Input 1: right 640×448 RGB888 planar
- Output 0 (larger): cost volume [128, 224, 320] uint8 (~9.2MB)（当前 v1.7 模型；旧 96 通道模型已不用）
- Output 1 (smaller): integer disparity [224, 320] uint8 (~70KB)
- 输出通过 size 自动检测: 大的是 cost, 小的是 disp

### 当前串行输出模式（已去除NPU输出双缓冲）
- `STEREO_NPU_BUF_SETS = 1`，只保留 1 组 SMR cached output buffer
- `cur_buf_set` 始终为 0，不再 ping-pong
- 不再维护 `g_npu_buf_in_use[]`，`stereo_npu_release_buf_set()` 为兼容 no-op
- SubPixel 在 NPU 线程内同步读取 cost/disp 并完成 Q5 视差精化，完成后才允许下一次 NPU 推理复用输出 buffer
- 好处：避免 NPU 覆盖 SubPixel 尚在读取的 cost/disp buffer，降低数据闪烁风险；代价是 NPU 与 SubPixel 不再并行

### CVE 输入双缓冲（identity map 输出）
- `STEREO_CVE_NPU_BUF_NUM = 2`，CVE map 输出到 2 套独立 buffer，乒乓切换
- NPU 输入队列深度为 1（`STEREO_NPU_QUEUE_DEPTH = 1`），确保 CVE 完成一帧后 NPU 才开始处理
- 消除 CVE 就地 map 操作与 NPU 读取之间的数据竞争（闪烁根因）
- 减128预处理已迁移至 ORTM DTC Preprocess 节点，CVE 输出原始 uint8 RGB888
- 详见"CVE identity map 双缓冲"章节

### 性能优化要点
1. **禁止**: 在non-cached内存上CPU逐字节操作 (XOR 0x80)
2. **最优**: CVE RGB888 planar 输出直连 NPU 输入，零拷贝
3. **实现**: `ot_smr_alloc(cached=OT_TRUE)` + `ot_smr_flush_cache`
4. **实测**: NPU=49ms/帧 (包含 cache flush)

### 关键调用
```c
ot_avp_npu_set_buffer(input_dataset, idx, virt, phys, size, stride);
ot_avp_npu_execute(model_hdl, input_dataset, output_dataset[cur_set]);
ot_smr_flush_cache(phys, virt, size);  // 推理后 invalidate cache
```

---

## 模块3: CVE 图像预处理

### 功能链
VPROC输出 1280×1080 NV12/NV21 → 下采样 640×540 → Crop 640×448 (居中裁剪, top=46) → CSC → RGB888 planar → identity map (uint8直传, DTC处理减128)

### 关键: CVE map identity LUT（uint8 RGB888 直传）
减128预处理已迁移至 ORTM 模型的 DTC Preprocess 节点。CVE CSC 输出后用 `ot_avp_cve_map` 算子对 3 个平面分别应用 identity LUT（`lut[i] = i`），将数据拷贝到独立 NPU 输入 buffer，保持双缓冲架构。详见上方"CVE identity map 双缓冲"章节。

### 关键: CVE buffer直传NPU
CVE map 输出 buffer 的 phys/virt addr 可直接赋给 NPU 输入，无需额外拷贝。map 输出到双缓冲 buffer（`g_cve_npu_*[buf_idx]`），通过 `stereo_cve_process()` 的 `left_crop`/`right_crop` 出参返回。

### 尺寸常量 (stereo_types.h)
```c
#define STEREO_SENSOR_WIDTH         1280
#define STEREO_SENSOR_HEIGHT        1080
#define STEREO_CVE_DOWNSAMPLE_W     640
#define STEREO_CVE_DOWNSAMPLE_H     540
#define STEREO_CVE_CROP_W           640
#define STEREO_CVE_CROP_H           448
#define STEREO_NPU_INPUT_W          640
#define STEREO_NPU_INPUT_H          448
#define STEREO_CROP_TOP             46
#define STEREO_NPU_OUTPUT_W         320   // NPU 原生输出宽
#define STEREO_NPU_OUTPUT_H         224   // NPU 原生输出高
#define STEREO_DISP_OUTPUT_W        640   // 上采样后传输/显示宽
#define STEREO_DISP_OUTPUT_H        448   // 上采样后传输/显示高
```

---

## 模块4: SubPixel 亚像素视差精化

### 核心文件
- `stereo_subpixel.c` — 5-tap抛物线拟合，整数算术优化
- `stereo_subpixel.h` — 接口定义

### 算法
对每个像素，取整数视差 d，用 cost volume 中 d-2..d+2 的 5 个值做非对称抛物线拟合，输出 Q5 定点 uint16（real_disp = val/32）。

### 性能优化要点 (当前实测约5-9ms/帧)
1. **整数算术**: 全 int32 运算，消除 FPU stall (Cortex-A55 SDIV 8-12 cycles)
2. **4行批量处理**: 同一 channel 的4行连续读取（地址相邻，减少 page miss）
3. **channel prefetch**: `__builtin_prefetch` 预取下一个 channel 数据
4. **静态 workspace**: 120KB static buffer，避免每帧 malloc/free
5. **批量行范围扫描**: 每4行统一扫描 d_min/d_max，减少 channel 加载数

### VPROC GDC DMA 带宽竞争
- XY-LUT 启用后 VPROC GDC 硬件 DMA 持续运行 (~144MB/s)，可能与 CPU 读取 cost volume 竞争 DRAM 带宽
- 曾观察到 SubPixel 在 GDC DMA 竞争下明显退化；当前通过批量行 + prefetch 将实际耗时压到约5-9ms

### 关键约束
- cost volume 布局: channel-first [96, 240, 320]，每个 channel 76800B
- 相邻 channel 间距 76800B（散乱读取，硬件预取器无效）
- 必须用批量读取减少 DRAM 访问次数

---

## 模块5: VENC 编码

### 核心文件
- `stereo_venc.c` — MJPEG编码 + LZ4 disparity打包
- `stereo_venc.h` — 接口

### 帧率配置 (20fps)
```c
attr->mjpeg_attr.rc_attr.output_fps = 20;
attr->mjpeg_attr.rc_attr.input_fps  = 20;
out_frame->pts = (ot_u64)frame_seq * 50000;  // 50ms PTS间隔
```

### 关键决策
- **左右图**: MJPEG硬编码 (ch0, ch1)，qfactor=75（从90降低以减少传输量约65%）
- **Disparity**: uint16 Q5 LZ4压缩 (不经VENC硬件编码)
- **帧类型**: 0x00=left, 0x01=right, 0x03=disparity_lz4, 0x06=calib_json, 0x07=perf_json

**坑**: 对 disparity 使用 VENC 硬编码会导致 buffer pool 争用 → 帧率跌至 6.67fps。

**JPEG质量优化**: qfactor 从 90 降至 75，每帧数据量从 ~1240KB 降至 ~440KB（-65%），显著降低网络传输延迟。

### 协议头 (24字节, big-endian)
```
Format: '>IBIQI3s'
Fields: magic(4B) | frame_type(1B) | frame_index(4B) | timestamp_us(8B) | payload_size(4B) | reserved(3B)
```

### 日志规范
- 连接/断开: 每次打印
- 数据传输: 仅每秒汇总 FPS，禁止逐帧日志

### LZ4 用户空间压缩库（stereo_lz4.c/h）

**背景**: 板端没有现成的 LZ4 用户空间库，从 Linux 5.10 内核源码 (`lib/lz4/`) 适配为用户空间实现，用于 disparity 数据压缩传输。

**核心文件**:
- `stereo_lz4.h` — API 定义、常量、`LZ4_stream_t` 结构
- `stereo_lz4.c` — 压缩/解压实现（适配自 `lz4_compress.c`、`lz4_decompress.c`、`lz4defs.h`）

**API**:
```c
int LZ4_compress_default(const char *src, char *dst, int srcSize,
                          int maxDstSize, void *wrkmem);
int LZ4_decompress_safe(const char *src, char *dst, int compressedSize,
                         int maxDecompressedSize);
int LZ4_compressBound(size_t isize);  // inline
```

**内存需求**: `LZ4_MEM_COMPRESS = LZ4_STREAMSIZE = 16416 bytes` (wrkmem)

**关键坑点 — LZ4_NbCommonBytes off-by-one 导致 XOR 0x80 腐蚀**

**根因**: 从内核适配时，`LZ4_NbCommonBytes()` 函数使用了 `__builtin_ffsl(val) >> 3`。`ffsl` (find first set long) 返回 **1-indexed** 位置，而 `ctzll` (count trailing zeros) 返回 **0-indexed** 位置。当差异位恰好落在字节最高位 (bit 7, 15, 23, ..., 63) 时，`ffsl >> 3` 会多算 1 字节，导致 match 长度 +1。这会在解压输出中引入一个 XOR 0x80 的错误字节。

**表现**:
- 板端自身 LZ4 round-trip（压缩→解压）就有 ~2-6% 字节不匹配
- 板端压缩 → Python lz4 解压，出现 100% XOR 0x80 差异模式
- 视差图中部分像素值的高字节 bit 7 被翻转，产生异常值

**修复**:
```c
// 修复前 (错误) — ffsl 返回 1-indexed，off-by-one
static FORCE_INLINE unsigned int LZ4_NbCommonBytes(register size_t val)
{
#if LZ4_LITTLE_ENDIAN
    return __builtin_ffsl(val) >> 3;       // BUG
#else
    return (8 * sizeof(long) - 1 - __builtin_clzl(val)) >> 3;  // BUG
#endif
}

// 修复后 (正确) — ctzll 返回 0-indexed，直接右移3位得字节索引
static FORCE_INLINE unsigned int LZ4_NbCommonBytes(register size_t val)
{
#if LZ4_LITTLE_ENDIAN
    return __builtin_ctzll(val) >> 3;      // FIX
#else
    return __builtin_clzll(val) >> 3;      // FIX
#endif
}
```

**验证方法**:
1. 板端 round-trip 测试：在 VENC 线程中对 disparity 先压缩再解压，逐字节比对
2. 端到端验证：板端 LZ4 压缩 → TCP 传输 → Python `lz4.block.decompress` 解压，检查 disparity 值域和 XOR 0x80 模式
3. 验证脚本: `scripts/verify_lz4_fix.py` — 无头采集 N 帧 disparity，检查高位字节、值域、帧间一致性

**验证结果**:
- 5 帧 disparity 全部 CLEAN (0/66560 像素高位异常)
- Python lz4 解压全部成功，压缩率 ~36% (48KB vs 133KB)
- disparity 值域 [0-3040] Q5 = [0.0-95.0] 无异常

**经验教训**: 从内核源码适配用户空间时，必须注意 `__builtin_ffsl` (1-indexed) 与 `__builtin_ctzll` (0-indexed) 的差异。内核源码中 `ffs` 和 `ctz` 的语义在不同架构和编译器下可能不一致，适配时应优先使用 0-indexed 的 `ctzll/clzll` 并直接右移。

---

## 模块6: 标定文件格式

### stereo_calib.json 标准格式
```json
{
  "version": 1,
  "distortion_model": "8-param rational",
  "image_size": [1280, 1080],
  "left":  { "fx":..., "fy":..., "cx":..., "cy":..., "distortion": [k1,k2,p1,p2,k3,k4,k5,k6] },
  "right": { "fx":..., "fy":..., "cx":..., "cy":..., "distortion": [k1,k2,p1,p2,k3,k4,k5,k6] },
  "extrinsics": { "rotation": [[...]], "translation_mm": [...], "baseline_mm":... },
  "rectification": { "R1":..., "R2":..., "P1":..., "P2":..., "Q":... }
}
```

### 标定文件来源
- 仓库基准文件：`main/src/component/media/sample/stereo_app_bk/scripts/stereo_calib.json`
- 板端运行文件：`/opt/stereo/stereo_calib.json`

---

## 模块7: 上位机 Receiver（stereo_receiver.py）

### 核心文件
- `main/src/component/media/sample/stereo_app_bk/scripts/stereo_receiver.py` — 主接收脚本

### 网络协议帧类型

| type | 名称 | 说明 |
|------|------|------|
| 0x00 | left | MJPEG编码左目矫正图 |
| 0x01 | right | MJPEG编码右目矫正图 |
| 0x02 | disparity | raw uint8视差（未使用） |
| 0x03 | disparity_lz4 | LZ4压缩uint16 Q5视差 (640×448) |
| 0x04 | raw_left | raw-only模式左目NV21 |
| 0x05 | raw_right | raw-only模式右目NV21 |
| 0x06 | calib | 标定JSON（连接后发送一次） |
| 0x07 | perf | 性能JSON（每帧发送） |

### 标定帧（type=6）字段
板端连接后自动解析 `/opt/stereo/stereo_calib.json`，缩放后发送：
```json
{
  "fx": 651.0,          // fx_orig / scale_x (scale_x=2.0)
  "cx": 646.2,          // cx_orig / scale_x
  "cy": 490.4,          // (cy_orig - crop_y_offset) / scale_x
  "baseline_mm": 60.0,
  "disp_w": 640, "disp_h": 448,
  "q5_scale": 32, "max_disp": 96,
  "image_w": 1280, "image_h": 1080,
  "scale_x": 2.0, "crop_y_offset": 60,
  "disp_scale": 1.0      // 传输分辨率=NPU输入分辨率，无需缩放
}
```

### 视差测距与 disp_scale 修正

**关键变更**: 板端已将视差从 320×224 上采样至 640×448 传输。标定 JSON 参数已适配：
- `scale_x = 2.0`（1280/640），fx/cx/cy 缩放到 640×448 空间
- `disp_scale = 1.0`（传输分辨率 = NPU 输入分辨率，无需额外缩放）
- `disp_w = 640`, `disp_h = 448`

```python
disp_scale = self.calib.get('disp_scale', 1.0)  # 上采样后无需缩放
disp_real = disp_raw / q5       # Q5定点转浮点（640空间）
disp_eff = disp_real / disp_scale  # disp_scale=1.0，无操作
Z = fx * baseline / disp_eff
X = (x_model - cx) * baseline / disp_eff
Y = (y_model - cy) * baseline / disp_eff
```

**历史背景**: 上采样前视差为 320×240，disp_scale=2.0，未修正时 Z 仅为正确值的一半（误差约50%）。上采样后 disp_scale=1.0，不再需要此修正。

### 采集保存优化
- **左右图**: 接收线程存储原始 JPEG 字节，保存时直接写入文件（`wb` 模式），避免 `cv2.imwrite` 二次编码导致质量损失
- **视差**: uint16 Q5 以 PNG 无损压缩保存，保留定点原始值

### 三线程流水线架构（receiver → decode → display）

上位机采用 **三线程流水线** 最大化吞吐：接收线程只做 TCP 读取，解码线程做 JPEG 解码（`cv2.imdecode` 释放 GIL，可与显示线程真正并行），主线程做视差伪彩色+合成+显示。

```
┌─────────────────┐   ┌──────────────────┐   ┌──────────────────┐
│ Receiver Thread  │   │  Decode Thread    │   │  Main Thread      │
│ (TCP read only)  │   │  (JPEG decode)    │   │  (display)        │
│                  │   │                   │   │                   │
│ read_frame()     │   │ cv2.imdecode()    │   │ build_display()   │
│ 存原始JPEG字节   │──→│ 存解码后numpy数组 │──→│ LUT+composite+    │
│ 覆盖_latest_raw  │   │ 覆盖_latest_     │   │ imshow+waitKey    │
│                  │   │   decoded         │   │                   │
│ if old unread:   │   │ if old unread:    │   │ if no new frame: ││
│  drop_count++    │   │  drop_count++     │   │  waitKey(10)     │
│  overwrite       │   │  overwrite        │   │                   │
└─────────────────┘   └──────────────────┘   └──────────────────┘
       ↑  _raw_lock                ↑  _decoded_lock
       ↑  _latest_raw              ↑  _latest_decoded
```

- **接收线程**：只做 `read_frame()` + 连续采集保存，存原始 JPEG 字节到 `_latest_raw`（`_raw_lock` 保护），旧帧未消费则 `drop_count++`
- **解码线程**：从 `_latest_raw` 取最新帧，`cv2.imdecode(np.frombuffer(...), IMREAD_REDUCED_COLOR_2)` 解码为 numpy 数组，存到 `_latest_decoded`（`_decoded_lock` 保护）。**关键：`cv2.imdecode` 释放 GIL，可与主线程的 numpy 运算真正并行**
- **主线程**：从 `_latest_decoded` 取最新解码帧，执行 `build_display()` + `cv2.imshow()` + `cv2.waitKey()`
- 信息面板显示 `Dropped: N` 计数（raw + decoded 两级丢帧总和）
- 连续采集在接收线程中执行，保证每帧都保存
- TCP_NODELAY 已设置，减少小包延迟
- **TCP SO_RCVBUF 限制为 200KB**：防止内核缓冲区累积~12帧（默认6MB），强制 TCP 背压使板端丢帧而非上位机累积陈旧帧

### 显示性能优化（6项，9.4→15.5 FPS）

通过 timing instrumentation 精确定位瓶颈后，实施以下优化：

| 优化项 | 原理 | 节省 | 代码位置 |
|--------|------|------|----------|
| **LUT 查表** | 预计算 Q5→BGR 查找表 (65536×3)，单次 numpy 索引替代 5 次/帧数组运算 (astype/div/clip/applyColorMap/mask) | ~5ms | `_build_disp_lut()` + `build_display()` |
| **预分配组合缓冲区** | 1600×1016×3 buffer 用 slice 赋值替代 6 次 `vstack`/`hstack`，消除 ~15MB/帧临时分配 | ~8ms | `__init__` 中 `self._composite` + `build_display()` |
| **缓存 label bar** | 预生成 3 个静态标签条，避免每帧 `make_label_bar()` | ~1.5ms | `__init__` 中 `self._label_l/r/d` |
| **延迟 disparity.copy()** | `build_display` 只存引用，鼠标点击时才 `.copy()` | ~1ms | `build_display()` + `on_mouse()` |
| **三线程流水线** | JPEG 解码移至独立线程，与显示线程并行（`cv2.imdecode` 释放 GIL） | ~28ms (解码并行化) | `_decode_loop()` + `run()` |
| **--scale 选项** | `WINDOW_NORMAL` + 可选 `cv2.resize` 缩小显示窗口 | 无显著提升（imshow 固定开销与尺寸无关） | `display_frames()` + `main()` |

**优化效果**：
```
优化前: 9.4 FPS, 263 dropped (33.2s)
优化后: 15.5 FPS,  75 dropped (34.6s)
提升:   +65% FPS,  -71% 丢帧
板端:  ~19 FPS
```

**剩余瓶颈**：`cv2.imshow` 的 ~29ms Qt/X11 固定渲染开销（与图像尺寸无关），这是 15.5→19 FPS 差距的主要原因。`cv2.WINDOW_OPENGL` 在当前 conda OpenCV 构建中不可用（崩溃）。如需进一步提速，可考虑 pygame/ImGui 等 OpenGL 后端替代 `cv2.imshow`。

### 延迟分析
- **TCP协议本身延迟 <1ms**（本地网络），非延迟主因
- **主要瓶颈**: NPU推理 49ms + SubPixel 9ms + JPEG解码+渲染 + 数据量
- **优化措施**:
  - JPEG qfactor 90→75：每帧 1240KB→440KB（-65%）
  - TCP_NODELAY：消除 Nagle 算法延迟
  - 三线程流水线：防止 TCP 缓冲区帧累积导致越来越大的延迟
  - LUT 查表 + 预分配缓冲区 + 缓存标签：减少每帧渲染开销

### 上位机 per-frame 渲染耗时分解

通过 timing instrumentation 测量的各阶段耗时（优化后）：

| 阶段 | 耗时 | 说明 |
|------|------|------|
| JPEG decode (IMREAD_REDUCED_COLOR_2) | ~28ms | 1280×1080→640×540 半分辨率解码，在 decode 线程并行执行 |
| Disparity LUT 查表 | ~1ms | `self._disp_lut[disparity]` 单次 numpy 索引 |
| 预分配 buffer slice 赋值 | ~3ms | 替代 vstack/hstack，无临时分配 |
| cv2.imshow | **~29ms** | Qt/X11 后端固定开销，与图像尺寸无关 |
| cv2.waitKey(1) | ~8ms | 事件循环处理 |
| 其他 (resize/overlay/info) | ~5ms | |
| **合计（串行部分）** | **~65ms** | 三线程流水线后 decode 与 display 并行，实际帧时间 ~44ms → ~15.5 FPS |

### LZ4解压兼容（无pip lz4模块时）
当系统缺少 `lz4` Python模块且 pip 安装失败时，可用 ctypes 直接调用系统 liblz4：
```python
import ctypes
lib = ctypes.CDLL('liblz4.so.1')
lib.LZ4_decompress_safe.restype = ctypes.c_int
lib.LZ4_decompress_safe.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
# decompressed = lib.LZ4_decompress_safe(src, dst, src_len, dst_len)
```

### 视差图显示策略
当前布局（2026-08-18 改版）：**左视图（目标检测图）放大到 960×672**，右视图保持 640×448（顶部对齐），**视差图缩小到 427×299（约 2/3）并居中**，合成窗口 1920×1027 + 右侧 320 宽信息面板。测距仍使用 640×448 原始数据，面板偏移/鼠标映射常量 `DISP_PANEL_OFFSET_X/Y`、`DISPARITY_DISP_W/H` 已同步更新，改布局时这些必须一起改。窗口放不下加 `--scale 0.8`。

**伪彩色渲染**：使用 JET colormap + min/max 归一化，视差值归一化到 [0, max_disp] 后映射到 0-255 色域，无效视差（<0.1）显示为黑色。优化后通过预计算 LUT 实现：
```python
# _build_disp_lut(): 预计算 Q5→BGR 查找表
# JET with min/max normalization (disp 0..max_disp → 0..255)
normalized = np.clip(q5 / DISPARITY_MAX_DISP, 0, 1)
idx = (normalized * 255).astype(np.uint8)
jet = cv2.applyColorMap(np.arange(256).reshape(256, 1), cv2.COLORMAP_JET).reshape(256, 3)
lut = jet[idx]  # (3073, 3) uint8
full_lut = np.zeros((65536, 3), dtype=np.uint8)
full_lut[:max_q5] = lut

# build_display(): 单次索引替代 5 次数组运算
disp_color = self._disp_lut[disparity]  # O(1) per pixel
```

**--scale 选项**：支持显示缩放，适合屏幕分辨率不足时缩小窗口：
```bash
python3 stereo_receiver.py --host 192.168.1.101 --scale 0.5  # 半分辨率显示
```
注意：`--scale` 对帧率无显著提升，因 `cv2.imshow` 的 Qt/X11 后端有固定渲染开销，与图像尺寸无关。

**conda cv2 5.x 无字体警告**：`npu` 环境的 cv2 用 Qt 后端，启动会打 `Qt no longer ships fonts / Cannot find font directory .../cv2/qt/fonts`，导致 cv2 自己画的文字（信息面板/FPS/测距文字）可能不渲染；**左图里的检测框标签/距离是板端画进 JPEG 的，不受影响**。修复：把 DejaVu 字体目录放到 `.../site-packages/cv2/qt/fonts/`。

---

## 模块8: 模型加密与设备绑定（安全模块）

### 功能定位
实现 ORTM 模型的设备绑定加密保护：每台设备的硬件 UID 不同，模型经 UID 派生密钥加密后只能在该设备上解密运行。启动时先验证设备授权（license.bin），再解密模型到内存加载。

### 安全架构
```
[上位机]                        [板端]
encrypt_model.c                stereo_sec.c / auth_gen.c
  ├─ HMAC-SHA256(uid, KEY)      ├─ ot_omi_sys_get_unique_id() → 24B UID
  ├─ AES key = HMAC[:16]        ├─ HMAC-SHA256(uid, KEY) → cipher engine
  ├─ OpenSSL AES-CTR-128加密    ├─ AES key = HMAC[:16]
  └─ 输出 .ortm.enc             ├─ ot_omi_cipher AES-CTR-128解密 (硬件)
      = [IV(16B)][ciphertext]    └─ ot_avp_npu_load_model_from_mem()
```

### 核心文件
| 文件 | 平台 | 说明 |
|------|------|------|
| `stereo_sec_key.h` | 共享 | MASTER_KEY（32B）、路径常量、尺寸常量 |
| `stereo_sec.h/c` | 板端(ARM) | 设备校验 + 模型解密，调用 ot_omi_cipher 硬件引擎 |
| `auth_gen.c` | 板端(ARM) | 独立授权生成器：读 UID → HMAC → 写 license.bin → 输出 UID |
| `encrypt_model.c` | 上位机(x86) | OpenSSL HMAC + AES-CTR-128 加密工具 |
| `gen_license.sh` | 上位机 | 自动部署 auth_gen 到板端并获取 UID 的脚本 |
| `tools/host/Makefile` | 上位机 | encrypt_model 编译规则，自动检测 OpenSSL |

### 关键常量 (stereo_sec_key.h)
```c
#define STEREO_SEC_MASTER_KEY_LEN  32
static const unsigned char STEREO_SEC_MASTER_KEY[32] = { ... };  // 共享密钥
#define STEREO_SEC_LICENSE_SIZE     64   // 32B HMAC + 32B zero pad
#define STEREO_SEC_LICENSE_PATH     "/opt/stereo/license.bin"
#define STEREO_SEC_MODEL_ENC_PATH   "/data/model/stereo_match.ortm.enc"
#define STEREO_SEC_UID_BYTES        24   // ot_unique_id.id[6] → 24 bytes
#define STEREO_SEC_IV_SIZE          16
#define STEREO_SEC_HMAC_SIZE        32
#define STEREO_SEC_AES_KEY_SIZE     16   // HMAC[:16] as AES-128 key
```

### 加密模型文件格式
```
[IV (16 bytes, random)] [AES-CTR-128 ciphertext (N bytes)]
```
解密时先读 16B IV，再解密剩余部分，输出与原始 ORTM 完全一致的字节流。

### 板端集成点
1. **stereo_media.c** — NPU init 前调用 `stereo_sec_check_device()` 验证设备授权
2. **stereo_npu.c** — 用 `stereo_sec_decrypt_model_to_file("/tmp/stereo_plain.ortm")` 解密成临时明文文件，再 `ot_avp_npu_load_model()` 从文件加载、load 后立即 unlink。**不能用 `ot_avp_npu_load_model_from_mem()`：从内存加载的模型走异步 trigger/wait 会永久卡在 wait**（详见模块9）。

### 编译
```bash
# auth_gen (ARM aarch64, 交叉编译)
export PATH=/opt/linux/x86-arm/aarch64-otv02-linux-gnu-gcc/bin:$PATH
make -C main/src/component/media/sample/stereo_app_bk COMMON_DIR=$SDK/main/src/component/media/sample/common auth_gen

# encrypt_model (x86_64, 本机编译；系统 OpenSSL 可用时无需额外 LD_LIBRARY_PATH)
make -C main/src/component/media/sample/stereo_app_bk/tools/host
```

### 部署与验证流程
```bash
# 1. 部署 stereo_app 和 auth_gen 到板端
sshpass -p 123456 scp stereo_app root@192.168.1.101:/opt/stereo/stereo_app
sshpass -p 123456 scp auth_gen root@192.168.1.101:/tmp/auth_gen

# 2. 板端运行 auth_gen 获取 UID 并生成 license.bin
sshpass -p 123456 ssh root@192.168.1.101 "chmod +x /tmp/auth_gen && /tmp/auth_gen"
# 输出: UID:f4245ac778e6c321178728a40a03d8d46f9e030007000000
# 副产物: /opt/stereo/license.bin (64B)

# 3. 上位机用 UID 加密模型
./encrypt_model \
  /home/lzx/lq560v100_sdk/stereo_s_ori_h448_w640_128_sub_v1.7_e300_sim.ortm \
  /tmp/stereo_match.ortm.enc \
  <auth_gen 输出的 UID>

# 4. 部署加密模型到板端（目录不存在时先创建）
sshpass -p 123456 ssh root@192.168.1.101 "mkdir -p /data/model"
sshpass -p 123456 scp /tmp/stereo_match.ortm.enc root@192.168.1.101:/data/model/

# 5. 运行 stereo_app 验证
sshpass -p 123456 ssh root@192.168.1.101 "cd /opt/stereo && ./stereo_app"
```

### 成功日志标志
```
media init success!                              # 媒体初始化（含 stereo_sec_check_device 通过）
model verification successful                    # NPU 模型解密+校验成功
load model success.                              # NPU 模型加载成功
```
注意：上述标志来自 sample_comm_sys 的 printf 与 NPU 运行时，不经过被禁用的 `stereo_log_write()`，因此“运行时日志已禁用”后仍会打印；板端无输出反而说明启动在更早阶段失败。

### 已知问题与注意事项
- **crypto_ioctl failed 0x13200007**: cipher 库内部非致命警告，不影响 HMAC/AES 结果，可忽略
- **驱动未加载时 auth_gen 失败**: `ot_omi_sys_get_unique_id` 返回 `0xa0028018`，需先执行 `/opt/ompmod/load_lq560v100 -i` + `/opt/kermod/load_kermod.sh`
- **UID 因设备而异**: 每次更换设备/相机模块，UID 都会变化，必须重新运行 auth_gen 并重新加密模型
- **/tmp 为 tmpfs**: auth_gen 部署到 /tmp 重启后丢失，每次需重新部署
- **OpenSSL 依赖**: 上位机若无 libssl-dev，Makefile 自动回退到 miniconda3，运行时需设置 LD_LIBRARY_PATH
- **MASTER_KEY 安全**: 生产部署前必须更换 `stereo_sec_key.h` 中的 MASTER_KEY

### 上电自动启动（user_init.sh）
板端 `/opt/user_init.sh` 已配置自动加载驱动并启动 stereo_app：
```sh
# 1. 加载内核驱动模块
cd /opt/ompmod && ./load_lq560v100 -i
cd /opt/kermod && ./load_kermod.sh
# 2. USB Gadget + SSH
echo device > /proc/10320000.usb30drd/mode
/opt/bin/usb-ether.sh
/data/openssh/start_sshd.sh &
# 3. 启动 stereo_app
cd /opt/stereo && ./run.sh &
```

---

## 模块9: 双模型集成（stereo 深度 + tiny-yolov3 检测，plan 3.6）

### 功能定位

在 stereo_app 现有 VI/VPROC(XY-LUT)/CVE/NPU/SubPixel/VENC/TCP9000 管线上扩展第二模型 tiny-yolov3：左 VPROC group 新增 chn2 检测通道（416×312 YUV420SP@10fps），letterbox 416×416 喂 yolo；NPU 改双模型 ABAB 异步调度（trigger stereo → trigger yolo → wait stereo → wait yolo）；检测框映射回左目 1280×1080 后由板端画进左图 JPEG，上位机 receiver 零改动。

### 核心文件

- `stereo_yolo.c/h` — yolo 模型加载、IO 运行时查询、416×312→416×416 letterbox 预处理、就地解码+NMS、画框。
- `stereo_npu.c/h` — stereo 模型改 trigger/wait 两段接口（`stereo_npu_trigger`/`stereo_npu_wait`），`thread_num=2`。
- `stereo_sec.c/h` — `stereo_sec_decrypt_model_to_file()`：加密模型解密成临时明文文件。
- `stereo_media.c` — chn2 检测通道配置 + 检测线程（单槽最新帧）+ npu 线程 ABAB 接线。

### 非协商 API 事实（均已板端实测）

- **`ot_avp_npu_load_model_from_mem` 的模型走异步 trigger/wait 会永久卡在 wait**（同步 execute 正常）。必须解密成 `/tmp/stereo_plain.ortm` 文件、`ot_avp_npu_load_model` 加载，load 后立即 unlink。yolo 明文模型在 `/opt/model/tiny-yolov3_yuv420sp_b.ortm`（`/data` 放不下）。
- NPU 输出禁整块拷贝、禁逐元素读 `ot_avp_npu_malloc` 内存：F32 视差 286KB 标量扫描 ~18ms/帧；保留 SMR cached 输出 + `ot_smr_flush_cache` 为 ~1.3ms。yolo 输出走 stride 对齐的就地懒读（先读 objectness，过阈才读 85 值组）。
- VPROC chn 级 crop 在 **chn 缩放之后**生效：全幅 1080×1280 先缩到竖版 352×416，再 chn crop (20,0,312,416) 等效源空间 960×1280@x=60；把 crop 写成源空间坐标会得到 416×252 错误输出。
- 检测通道 attr 竖版 312×416（270° 交换宽高、height 16 对齐）、`compress_mode=NONE`、FRC src=30 dst=10。本 app 名义 20fps 但实测传感器出帧 ~30fps（scene_auto 的 `ot_scene_set_static_ae` 按 `config_product_scene_1p5m30_built_in.ini` 的 `frame_rate=3000` 把 AE 帧率覆盖成 30），src=20 会得到 15fps。
- 检测线程单槽最新帧 + 互斥锁/条件变量，**拷贝即消费**；npu 线程**阻塞**等新检测帧（1s 超时兜底），整条流水线定拍在 10fps。**不要改回非阻塞取槽**：19fps JPEG 与 10fps yolo 错拍会让「有框/无框」两帧交替，框严重频闪；定拍后每帧都带新画的框，实测 10.02fps 稳定。
- 左帧是 YVU420SP（fmt 221），UV 行 stride=1280 字节，chroma 偏移 = `phys_addr[1]-phys_addr[0]`。USER 帧画框用 `ot_smr_mmap` + 写回 `ot_smr_flush_cache`，否则 VENC DMA 读不到。画框顺序必须在 npu_proc 推入 venc 队列之前。
- 框坐标映射：`x_l=x*1280/416`、`y_l=60+(y-52)*960/312`，越界截断；框色按 class_id 用黄金角色相生成不同饱和色（RGB→BT.601 YUV），框内原图不动（勿把框内 UV 填 128，否则亮场景下框内会发灰白）。
- 框中心距离：取框左右边缘中点在视差图（640×448，左图 x/2、y=(y-92)/2 映射）上采两个端点 Q5，线性拟合中点 → `Z_mm=fx_disp·baseline_mm/(q5/32)`，画「X.XXm」于框中央；q5<16（<0.5px，过远）或端点越界不显示。fx_disp=P1.fx/2、baseline 从 `/opt/stereo/stereo_calib.json` 读，与 receiver 同一套换算保证一致。
- 文字渲染：内置 5×7 位图字体放大 **3×**（15×21px），标签类别色底 + 按底色亮度自适应黑/白字，距离为深色底白字居中。字体表只有 A-Z/0-9/./空格/小写 m——**要显示别的单位/字符得先补 glyph，否则未知字符静默变成空格**（曾出现距离丢了 `m` 单位）。

### 实测基线（2026-08-18）

- 输出帧率固定 10.02fps（stereo 48ms + yolo ~14ms 在单 NPU 上串行，10fps 周期内有余量）；纯 stereo 基线 19.2fps / SubPixel 1.3ms。
- 5 分钟连续运行（155 采样/310s）：RSS 稳定 12.0MB（末 60s 持平），MemAvailable 9.4~25MB，CPU ~43% 单核。
- M2 探针实测：stereo 模型 = 2 输入 + **1 输出 F32 [224,320]**（半分辨率视差，非早期文档的 cost+disp 双输出）；合成对视差输出空间均值 12.06 = 输入空间 24px。
- 框中心距离与 receiver 同帧同坐标交叉验证一致（端点 4090/4094 → 中点 4092 → 0.14m）。

### 运维坑（3.6 期间实测）

- **同一时间只允许一个媒体实例**：完整链路运行时再开 `./stereo_app --raw-only`（或第二个实例）会 `ot_vrb_config failed` / `media deinit fail 0xa0028022`，并把正在跑的进程媒体栈卸掉，日志刷 `vproc_get_chn_frame return 0x80010007`（VPROC NO_START）后帧流停摆。回归 raw-only 必须在主进程停掉之后做。
- kill -9 / 异常退出会留下脏媒体状态，下次启动直接 `ot_vrb_config failed`；`reboot` 最干净（`load_lq560v100 -a` 有过把 RNDIS 也带断的记录，谨慎用）。
- 5 分钟资源监控：用 `/proc/[0-9]*/comm` 扫描拿 PID，采样 `VmRSS/MemFree/MemAvailable/utime+stime`；勿在监控期间跑任何第二个媒体命令，否则数据被污染。

### 构建/部署/运行

与「构建与部署」一致，只多一步 yolo 模型部署（见 dual_model_abab README「十」的完整命令）。

---

## 模块10: 固件升级与整机部署

### 升级包 vs 烧录表（先分清这两件事）

- `main/pub/lq560plusv100_image_glibc/upgrade_0x01000300.img`（内部 `fw_version=0x01000300`）是 **OTA 升级包**，实测只含 4 个组件：uboot、bl31、kernel、rootfs。升级只重写这四块启动/系统分区，**userfs(/opt) 和 data(/data) 不动，数据保留**。
- `main/pub/lq560plusv100_image_glibc/nand_burn_table.xml` 是 PC 烧写工具的**整片烧写表**：9 个分区布局 + 各分区镜像 + `<Env>` 段的 u-boot 环境变量（bootargs/bootcmd 含 `misc_check→nand_ota_upgrade` 的 OTA 逻辑）。用它整片烧写会清空所有分区数据，适合新板出厂。

### OTA 升级正确姿势（不是 scp+重启就完事）

升级包必须写进 UBI `upgrade` 卷并置 PENDING 标志，重启后 u-boot 才会烧写：

```sh
scp upgrade_0x01000300.img root@192.168.1.101:/tmp/upgrade.img
ssh root@192.168.1.101 "/usr/bin/ota_prepare /tmp/upgrade.img"   # 校验+写卷+置标志
ssh root@192.168.1.101 "reboot"                                  # u-boot 自动 nand_ota_upgrade
```

板端 rootfs 有 `upgrade_triggerd` 守护（`/etc/upgrade_triggerd`）可能自动监控 `/data/upgrade`，但不要只靠它，显式跑 `ota_prepare` 最稳。img 能改的是启动链+Linux 系统（含 kernel/rootfs 里的硬件驱动）；改不了 otfl/uflag/userfs/data 和芯片级 OTP 熔丝等一次性配置。

### 新的一模一样相机重新跑本 app（顺序不能少）

1. 基础固件：新板用烧写工具 + `nand_burn_table.xml` 整片烧写（或已带系统时按上面对照内核编译时间确认版本）；不一致再 OTA。
2. 网络：按 `board-network-setup` skill 部署 RNDIS+SSH（新板可能默认 UVC 模式）。
3. 通用资产直接拷贝：`stereo_app`、`/opt/model/tiny-yolov3_yuv420sp_b.ortm`、`/opt/stereo/param/sc132gs/`。
4. **设备绑定重做（旧板文件不可复用）**：新板 `/tmp/auth_gen` 取新 UID → 主机 `encrypt_model` 重新加密 → 生成新板自己的 `/opt/stereo/license.bin` 与 `/data/model/stereo_match.ortm.enc`。
5. **新相机重新标定**：`stereo_calib.json`、`lut_left.bin`、`lut_right.bin` 每台不同，复用旧标定会导致矫正与距离不准。
6. 重启验证：`cd /opt/stereo && ./stereo_app`，主机 `stereo_receiver.py` 检查左图框/距离、右图、视差。

完整命令清单在 dual_model_abab README「十一」。

---

## 常见错误码

| 错误码 | 含义 | 解决方案 |
|--------|------|----------|
| 0x80010000 | VPROC ILLEGAL_PARAM | XY-LUT 场景优先检查 header/data 长度、X方向保护列、dst 16对齐 |
| 0xa0028018 | ot_omi_sys_get_unique_id 失败 | 驱动模块未加载，先执行 load_lq560v100 -i + load_kermod.sh |
| 0x13200007 | crypto_ioctl 内部警告 | cipher 库非致命警告，不影响 HMAC/AES 结果，可忽略 |
| 0xa0088008 | VENC资源未释放 | 先reboot板子再运行 |
| 0x80010003 | VPROC NOT_SUPPORT | 3DNR等不支持的功能调用，可按具体模块判断是否非致命；当前矫正链路仅使用 XY-LUT |
| 0x80000005 | VI NO_ACCESS | scene_auto初始化时可能出现的警告；若 raw-only 已继续启动 raw_capture，可先按非致命处理 |
| 0x80010007 | VPROC NO_START | 正在跑时被另一媒体实例/raw-only 把媒体栈卸了；停掉多余实例，必要时 reboot |
| 0xa0028022 / ot_vrb_config failed | 媒体/VRB 脏状态或资源被占用 | kill -9 后常见；reboot 清状态再启动 |
| VPROC输出全黑 | XY-LUT坐标格式错误 | LUT数据区坐标必须为像素坐标×16(Q28.4)，不要用×65536(Q16.16) |
| raw header closed | 9001连接后无header | 查看板端 `/tmp/stereo_app.log`，重点查 VI dump、mmap、send_all 日志 |
| raw data size mismatch | RAW0 meta解析长度不一致 | Python端必须使用 `RAW_META_FMT = '>4sHHHHBBHII'` 24字节格式 |
| 测距Z值偏小约50% | 视差值在NPU输入分辨率(640)，fx/cx/cy在输出分辨率(320) | 已修复：板端上采样后传输分辨率=NPU输入分辨率(640×480)，disp_scale=1.0 |
| 帧累积延迟递增 | TCP流式传输+单线程接收，显示慢时旧帧堆积在缓冲区 | 上位机采用三线程流水线 (receiver→decode→display)，接收线程只保留最新帧 |
| VENC chn创建失败 0x80080052 | kill -9 后内核残留 VENC 通道 | 代码已自动恢复：检测 chn0 失败时 venc_exit + venc_init + 重试 |
| 上位机帧率远低于板端 | cv2.imshow Qt/X11 后端固定 ~29ms 渲染开销 + JPEG 解码 ~28ms 串行 | 三线程流水线 (decode 并行) + LUT 查表 + 预分配缓冲区，15.5 FPS（板端 ~19 FPS） |
| cv2.WINDOW_OPENGL 崩溃 | conda 安装的 OpenCV 未编译 OpenGL 支持 | 使用 WINDOW_NORMAL，或考虑 pygame/ImGui 替代 |
| LZ4解压后 XOR 0x80 腐蚀 | `LZ4_NbCommonBytes` 使用 `__builtin_ffsl` (1-indexed) 而非 `__builtin_ctzll` (0-indexed)，match长度多算1 | 改用 `__builtin_ctzll(val) >> 3`，详见 VENC 模块 LZ4 章节 |

---

## 性能基线 (实测, XY-LUT已启用)

| 阶段 | 耗时 | 备注 |
|------|------|------|
| VI帧获取 | ~50ms (20fps) | FSIN触发 |
| VPROC+XY-LUT | <5ms | GDC硬件查表, DMA占用约144MB/s |
| CVE预处理+XOR map | <4ms | 硬件加速 (resize+crop+CSC+map×6) |
| NPU推理 | **~47ms** | v1.7 128-sub 模型 + cache flush |
| SubPixel | **~1.4ms** | 整数算术 + 批量行 + prefetch |
| VENC | <5ms | MJPEG硬编码, qfactor=75 |
| 网络传输 | ~2ms | TCP本地, q=75时~440KB/帧 |
| **有效帧率** | **稳定优先模式：约取决于 NPU+SubPixel 串行耗时** | 当前去除双缓冲，吞吐不再按 max(NPU, SubPixel) 计算 |

### 当前验证基线
- 成功标志：左右均出现 `set_gdc(XY-LUT) OK`、`set_xylut OK` 和 `XY-LUT stereo rectification applied`
- 角点验证：当前实拍靶标按 `9×6` 内角点检测，直接测板端输出图 `mean|dy|≈1.0px`
- 模型标志：`model loaded: 2 inputs, 1 outputs`（v1.7 128-sub 为单输出 F32 视差；历史双输出模型才打印 2 outputs）
- 25秒运行可达到约 `total frames=400+`

---

## 关键参考信息源

| 类别 | 位置 |
|------|------|
| SDK API头文件 | `main/out/include/ot_eis_vproc_*.h` |
| VPROC XY-LUT示例 | `sample/vproc/sample_vproc.c` (GDC 2D-LUT sample) |
| VPROC公共流程 | `sample/common/sample_comm_vproc.c` |
| 错误码定义 | `out/include/ot_eis_errno.h`, `ot_eis_vproc_err.h` |
| 结构体定义 | `out/include/ot_eis_vproc_common.h` |
| binary字符串分析 | `strings libot_vproc.a \| grep "Invalid"` |
| VPROC开发参考 | `ReleaseDoc/zh/01.software/board/OMP/SD3589CV100 VPROC开发参考.pdf` |
