# 3.3 相机模块接入 NPU 检测流水线计划

## 摘要

在 `dual_model_abab` 内新增相机数据源：复用现有 `uvc_app` 的 sc132gs 双目媒体管线，但不创建/绑定 UVC gadget；左眼经中心裁剪 4:3（1080×810 区域）缩放到 640×480、VPSS 降帧到 10fps，以最新帧拉取语义喂给 tiny-yolov3 检测模型，推理结果继续走 SSH 流式传输。分类模型（A）本阶段保持不变，后续被双目立体匹配模型替换。

## 关键决策（已锁定）

- USB 端口：保持 RNDIS+ACM，不动；usb0/SSH 传结果，ttyGS0(COM7) 留给 upgrade 触发，不建 UVC gadget。
- 几何：检测通道 = 传感器中央 4:3 区域（960×1280@x=60，等价 1080×810 中心裁剪）经 270° 旋转后缩放为 640×480，无畸变。
- 帧率：传感器保持 30fps，vproc 通道 FRC dst=10。
- 同步：采集线程阻塞 `ot_eis_vproc_chn_acquire_frame` 只保留最新一帧；推理侧需要输入时取最新帧、丢弃积压帧；队列上界 1，内存有界。当前推理 ~6.7ms/帧快于 10fps 相机，不会积压。
- 数据流：帧不落盘（`/tmp` 是 50.7MB RAM 盘，板子总 RAM 约 101MB）；vproc 帧池 → 按 stride 拷入 NPU 输入 tensor → 立即 release。仅调试开关允许单帧快照到 `/tmp`。
- 检测模型输入固定 640×480 YUV420SP（460800B），与本方案输出一致；双目原生左右帧保留给对齐验证与后续立体匹配。
- 检测用左眼；双目左右帧按 PTS 配对（容差 ≤16.6ms，硬件 master/slave 同步预期 ΔPTS≈0）。

## 实现改动

### 相机源模块（新文件 camera.c/camera.h）

- `camera_init()`：照 `uvc_media.c` 顺序执行 `sample_comm_sys_init` → `set_vi_vproc_init_cfg(OFFLINE)` → `media_pipe_init` → `ot_buffer_pool_init` → VI start → vproc group/chn 创建 → `sample_comm_vi_bind_vproc` → `sample_uvc_scene_auto_start("./param/sc132gs", ...)`；不调用任何 UVC 注册/open/run。
- 左 vproc group 上新增检测通道：输出 640×480、`OT_EIS_IMAGE_FORMAT_YUV_420_SEMIPLANAR`、`compress_mode=0`、`frc={src=30,dst=10}`、队列深度 2；复用 group 裁剪 960×1280@(60,0) + 270° 旋转。
- 采集线程：先清空两路积压；循环取左/右原生帧做 PTS 配对并释放，同时取检测通道最新帧存入单槽缓存（互斥锁保护）。
- 对外接口：`camera_start/stop`、`camera_copy_latest_to_tensor(model_slot*)`（按 `img.buff.stride[part]` 逐行拷贝 Y/UV 平面到输入 tensor）、`camera_alignment_stats()`。

### dual_model_abab 集成（修改 main.c）

- CLI：`inputB == "camera"` 时启用相机源喂检测模型；A 保持文件输入不变；新增 `--dump-frame` 调试开关（仅单帧到 `/tmp`）。
- 主循环：`model_preprocess(B)` 相机分支改为 `camera_copy_latest_to_tensor`；ABAB trigger/wait 与后处理不变。
- 检测后处理：把 416 空间检测框按参考样例 `rescale()` 映射回源分辨率 640×480，再打印/NMS/流式输出；RESULT 帧追加 `src_w/src_h`。
- 退出路径：先 `camera_stop()` 再 `model_destroy`，避免线程持有媒体句柄。

### 里程碑（每步 ≤200 行、≤5 文件、编译通过后提交）

1. 将本计划写入 `/home/lzx/lq560v100_sdk/plan.md`。
2. 独立探测样例 `camera_probe`：启动 sc132gs 管线（无 UVC），验证 640×480/YUV420SP/stride/compress_mode、10fps 实际帧率、左右 ΔPTS，并确认不干扰 ether UDC。
3. 相机源模块 + 对齐统计接入 dual_model_abab。
4. 端到端：相机 → tiny-yolov3 → 后处理/流式，静态场景下与文件输入结果一致。
5. 资源验证：10fps 长时间运行测 RSS/CPU/延迟，确认无内存增长。

## 测试计划

- 探测样例跑 ≥30s：应采到约 300 帧，帧率为 10fps±1，输出格式/stride 符合预期，ΔPTS 分布 ≈0，进程 RSS 稳定。
- 一致性：同一静止画面，相机路径与 640×480 YUV420SP 文件路径的检测框（rescale 后）逐帧一致。
- 时序：每帧 `duration_us` ≈100ms（10fps 周期），SSH 主机端 10 条结果/秒、无 seq gap/CRC 错误。
- 内存：连续跑 5 分钟，`mem_usage.log` RSS 无单调增长，`/tmp` 与 `/data` 占用不增长。
- USB：运行期间升级触发端口（ttyGS0）可正常收发，ether/SSH 不受影响。

## 假设与默认

- SC132GS 现有 ISP/scene_auto 调优参数直接复用，不重新调 ISP。
- 双目立体匹配模型接口未知，本期只保留原生左右帧取流与对齐统计，不做深度后处理。
- 分类模型 A 在立体匹配模型就绪前继续用文件输入，ABAB 结构不拆。
- 若探测发现 vproc 输出带压缩/tile，先在该通道强制线性无压缩；若模型输入张量不是 640×480，按实际查询值调整通道分辨率或重转模型。

# 3.4 上位机 Demo：SSH 接收 / 解码 / 渲染显示

## 摘要

在 `dual_model_abab` 相机模式基础上新增上位机演示程序：板端把相机画面与检测结果经 SSH stdout 流式发送（新增图像帧类型），WSL2 上位机用 Tkinter+PIL 实时解码并渲染——窗口显示相机画面、目标检测锚框、分类 top-k、帧率/时延/错误统计，关闭窗口或点击 STOP 时经 stdin 控制帧让板端干净退出。不新建端口或服务，完全复用现有 SSH 通道与帧协议。

## 关键决策（已锁定）

- 运行环境：WSL2 本机（tkinter/PIL/numpy/paramiko 已就绪），GUI 用 Tkinter+PIL，不引入 PyQt/cv2。
- 传输：新增 `TRANSFER_TYPE_IMAGE`（type=7）+ stream 级别 `3 = 结果 + 相机图像`；`2` 仍只表示输出 tensor，语义不回改。图像按原始 NV12 发送、不做 zlib（板端压缩 194KB 约需数十 ms，得不偿失）。
- 图像内容：发送「模型实际看到的内容」——416×312 有效区（不含上下各 52 行灰边），从 B 模型输入 tensor 直接截取（Y 行 52..363、UV 行 26..181），零额外相机拷贝。
- 显示：416×312 NV12 → RGB → 缩放 640×480；检测框已在 640×480 源坐标系（RESULT 附 src_w/src_h），直接按 640×480 画布叠加。
- 时序/内存：主机侧单帧队列（容量 2，丢弃旧帧防积压）；渲染节奏跟随相机帧率，默认 `camera_fps=10`。
- 生命周期：GUI 内用 paramiko 开 SSH 通道（`makefile("rb")` 读取、原始通道无 PTY）；关闭窗口先发 CONTROL STOP 再断开。

## 实现改动

### 板端（修改 transfer.h + main.c）

- `transfer.h`：`TRANSFER_TYPE_IMAGE = 0x07`。
- `main.c`：解析 `stream_level >= 3` 为 `stream_image`（仅相机模式生效）；新增 `stream_send_image(tx, &models[1], frame)`：`model_id=2`、`seq=frame`，payload = `kind u8(1=NV12) + pad u8×3 + width u32 + height u32 + 416*312*3/2 原始 NV12`，`compress=false`；在 `camera_copy_latest_to_input` 成功后立即发送（保证图像与同 seq 结果对应）。
- 复用静态/栈缓冲避免每帧 malloc；发送失败按现有流错误路径退出。
- 现有 stream=0/1/2 与 `camera_stream.sh` 行为完全不变。

### 主机端（修改 host/npu_stream_receiver.py + 新增 host/npu_gui.py）

- 接收器：新增 `T_IMAGE=7`、`parse_image(payload)`（返回 w/h/nv12），并加 `--save-frames` 调试开关把图像帧落盘 `.yuv420sp`，供无 GUI 验证。
- GUI `npu_gui.py`（Tkinter+PIL+numpy+paramiko，跨步提交保持 ≤200 行/步）：
  - CLI：`--host/--user/--password/--fps/--board-dir`，默认 192.168.1.101/root/123456/10/`/data/npu_demo`；构造板端命令 `sample_dual_model_abab <A模型> <A输入> <B模型> camera <repeat=1000000> none 3 <fps>`。
  - 读线程：复用 `npu_stream_receiver.iter_frames/parse_result/parse_image`；NV12→RGB（numpy 平面拆分+上采样+矩阵转换）→ PIL 缩放 640×480 → 放入丢旧队列。
  - 渲染线程（`root.after`）：画布显示图像；按 seq 匹配的 detect RESULT 画锚框（`class=<id> score=` 文本）；侧栏显示 seq、duration_ms、接收 fps、A 模型 top-5、CRC/resync 计数。
  - 控制：STOP 按钮/窗口关闭 → 编码 CONTROL STOP 帧写入通道 stdin → 等待板端退出 → 关闭通道；板端残留（崩溃）时提示执行 `/opt/ompmod/load_lq560v100 -a`。
  - 错误处理：resync/CRC 计数展示不中断；SSH 断开弹窗退出。

### 文档

- `plan.md` 新增本章节。
- README「九、相机模式」补上位机用法与实测数据。

## 接口 / 协议变更

- 新帧类型：`type=7`（IMAGE），payload 前 12 字节 `[kind=1, pad×3, w u32, h u32]` + `w*h*3/2` NV12（416×312=194,688B），不压缩、不落盘。
- stream 级别：`0` 关、`1` 结果、`2` 结果+输出张量、`3` 结果+相机图像（相机模式）；`3` 不自动包含张量。
- 主机 CLI：`python3 host/npu_gui.py [--fps N] [--password P]`；`npu_stream_receiver.py --save-frames`。

## 里程碑（每步 ≤200 行、≤5 文件、编译/验证后提交）

1. 将本章写入 `/home/lzx/lq560v100_sdk/plan.md`（本步）。
2. 板端 IMAGE 帧 + stream=3：transfer.h 枚举 + main.c `stream_send_image`，交叉编译通过。
3. 接收器 `parse_image` + `--save-frames`：板端 stream=3 跑 30 帧，核对帧大小/CRC/seq。
4. 上位机 `npu_gui.py`：接收→NV12 解码→渲染 + 锚框 + STOP；WSLg 实弹验证。
5. README 上位机小节 + 10/20/30fps 带宽/资源实测数据落盘。

## 测试计划

- 协议级：用已有 `/tmp/camera_frame.yuv420sp`（416×416 含灰边）构造合成 IMAGE+RESULT 帧离线注入，验证 NV12 解码、灰边剔除、640×480 缩放、锚框像素坐标正确。
- 板端→主机 CLI：`stream=3` + `--save-frames` 跑 30 帧，核对每帧 194,700B、CRC=0、seq 连续、图像统计量（亮区/灰边）符合相机画面。
- GUI 实时：WSLg 弹窗显示 ~10fps 流畅画面；有目标时锚框位置/尺寸与 results.jsonl 数值一致；关闭窗口后板端进程退出、SSH 会话结束、UDC 保持 configured。
- 带宽：10/20/30fps 三档实测主机接收帧率与板端 CPU（应仅多 ~0.3ms/帧拷贝），记录到 README；30fps 若 RNDIS 带宽吃紧则以 10/20 为推荐档。
- 回归：stream=1/2、`camera_stream.sh`、文件输入模式运行结果不受影响。

## 假设与默认

- 上位机默认在本 WSL2 运行；Windows 原生运行留作后续（代码仅依赖 stdlib+paramiko+PIL，可移植）。
- 相机当前画面低对比度、可能无检测目标：锚框正确性以合成帧测试为准，实景出框由用户对准目标后确认。
- v1 锚框文本用 `class id + score`，不内置 COCO 80 类名称；标签映射作为后续增强。
- 板端 `repeat` 用大值（1000000）+ STOP 控制实现「长跑直到关闭」，不新增无限循环模式。
- 本章按已落地的 416×312 实现（3.3 早期的 640×480 假设已按实际模型输入调整为 416×312+灰边）编写。
- skill 的 `references/camera-npu-pipeline.md` 同步更新作为后续可选步骤。

# 3.5 双目深度模型接入 dual_model_abab（文件输入先行）

## 摘要

用新到的双目深度模型 `stereo_s_ori_h448_w640_128_sub_v1.7_e300_sim.ortm` 替换 `dual_model_abab` 的模型 A（mobilenetv2 分类），B 保持 sc132gs 相机 + tiny-yolov3 检测不变。本阶段 A 先用**文件双目图片**喂入（左右两张 640×448 RGB888 planar），验证模型载入、推理与视差输出；相机双目数据（深度图 + 相机数据）在后续模型替换/管线合并时再接入。

## 关键决策（已锁定）

- 模型 I/O（依据 stereo_app 同系列模型与 .ortm 内 Preprocess 节点）：2 输入、2 输出；输入 0/1 均为 640×448 RGB888 planar UINT8（`[1,3,448,640]`，860160B/张），减 128 已固化在模型 DTC Preprocess 节点，软件不做色彩变换；输出按 size 区分：大者为 cost volume（128 层，`[128,224,320]` UINT8，约 9.2MB），小者为整数视差（`[224,320]` UINT8，0..128）。运行时一律查询 shape/dtype/size，不硬编码。
- 模型文件 `_sim.ortm` 在本 SDK 可直接 `ot_avp_npu_load_model()` 明文加载（stereo_app 仅为防拷贝而加密，加密/设备绑定是后续可选步骤，不在本期）。
- 输入文件：沿用 `get_input_file()` 多输入目录约定，目录内命名 `0`（左）、`1`（右）。
- 板端存储：28MB 模型超出 `/data`（空闲 11.6MB）与 `/opt`（空闲 10.3MB），本阶段放 `/tmp`（50.7MB tmpfs，重启即失）做验证；正式持久化方案待定。
- 合成验证对：左图为纹理+目标块，右图 = 左图内容左移 24px（`right[i,j]=left[i,j+24]`），预期视差≈24；用它验证「输入顺序、DTC 减 128、视差符号/值域」三点。
- ABAB 调度不变：A(stereo) 与 B(detect) 均 MEDIUM 优先级，trigger A→B、wait A→B。

## 实现改动（每步 ≤200 行、≤5 文件、验证后提交）

1. 本计划写入 `plan.md`（本步）。
2. 零代码基线：现有 `dual_model_abab` 二进制以 stereo 模型为 A、目录双目对为输入、tiny-yolov3 文件输入为 B，`repeat=1` 落盘模式跑通；核对 2in/2out、输出 size、合成视差≈24。
3. 板端代码：`model_detect_kind` 增加 stereo 识别（2 输出且大者为 UINT8 3D、小者为 UINT8 disp），新增 `stereo_preprocess()`（逐行拷贝两路文件，无换算）与 `stereo_postprocess()`（v1：不整块拷贝输出，统计 disp 值域/均值，供校验与后续流式）。
4. 流式传输：新增视差帧类型（先发整数 disp `[224,320]` 或 2× 上采样 uint16，方案按实测带宽定），主机接收器 + GUI 解析。
5. 上位机 GUI：深度伪彩渲染与检测框叠加，STOP/断开生命周期沿用 3.4 已落地机制。
6. A 接入相机双目数据：左右帧 PTS 配对喂入 stereo 模型（复用 camera.c 的左右原生帧取流），实现相机深度图 + 相机数据；此步在「换上新模型后」执行。

## 测试计划

- 合成对：disp 输出在有效区域内均值≈24±1，无效/边缘区域为 0 或模型定义的无效值；cost 大小 = 128×224×320。
- 板端资源：模型放 `/tmp` 时监控 RSS/free，确认 101MB RAM 板可跑双模型；若 OOM，先降级为「仅 stereo 模型」的最小探针样例。
- 回归：mobilenetv2 文件路径、相机检测 stream=1/2/3、GUI 在 stereo 接入前保持原样可跑。

## 假设与默认

- 新模型与 stereo_app 同系列（RGB planar 直传 + 模型内 DTC），输入约定沿用 stereo_types.h 的 640×448。
- `_sim` 命名不代表仅模拟器可用：stereo_app 的板端模型即由 `*_sim.ortm` 加密而来。
- 本期不做 subpixel/LZ4/VENC/加密，视差 v1 用模型原始整数输出。

# 3.6 stereo_app 双模型集成（stereo 深度 + tiny-yolov3 检测）计划

## 摘要

在 `stereo_app_bk` 现有 VI/VPROC(含 XY-LUT)/CVE/NPU/SubPixel/VENC/TCP9000 管线上扩展第二模型 tiny-yolov3：左目新增 VPROC 检测通道（960×1280@x=60 → 270° 旋转 → 416×312 YUV420SP，FRC 20→10），letterbox 到 416×416 喂 yolo；NPU 改双模型 ABAB 异步调度；检测框映射回左目 1280×1080 后由板端直接画进左目 YUV 帧，随现有 JPEG/TCP9000 链路发送。上位机 `stereo_receiver.py` 与现有传输/编码协议零改动，不自造协议。

## 关键决策（已锁定）

**用户确认的三项执行方案（2026-08-18）**：

- 双模型逻辑放哪个板端应用：**扩展 stereo_app**（以 `stereo_app_bk` 为基座，不动 dual_model_abab 的 SSH 传输）。
- yolo 检测框如何让现有上位机显示：**板端画进左图**（不新增协议帧类型，上位机零改动）。
- 里程碑风险控制顺序：**先内存探针**，OOM 立即停止汇报。

1. 基座 = 扩展 `stereo_app_bk`，复用现有 VI/VPROC/CVE/VENC/TCP9000 管线与传输/编码格式，不自造协议。
2. 检测框由板端画进左目帧，receiver 零改动，预期左图带框、右图/视差不变。
3. NPU 异步化：stereo 由同步 `ot_avp_npu_execute` 改 `trigger/wait` 两段；`config.thread_num=2`；两模型同优先级（MEDIUM），ABAB：trigger A(stereo)→B(yolo)、wait A→B；仅当检测通道有新帧时才 trigger yolo。stereo 保持 ~19fps、yolo ~10fps。
4. 检测通道：左 VPROC group 加 chn2。group crop 是 group 级共享（stereo 主链路占全幅 1080×1280），chn2 必须用 chn 级 crop `ot_eis_vproc_chn_set_crop`（960×1280@x=60），再 270° 旋转输出 416×312 YUV420SP、`compress_mode=NONE`、FRC src=20 dst=10（本 app 传感器 20fps，与 abab 样例 30fps 不同）；几何参数逐行照抄 `dual_model_abab/camera.c`。
5. 检测通道 attr 竖屏填写：width=312、height=416（270° 旋转交换宽高；height 必须 16 对齐，否则静默无帧）。
6. 单槽最新帧 + 互斥锁/条件变量；拷贝即消费（每帧 duration≈1000/fps）；letterbox 上下各 52 行 Y/UV=128 灰边（UV 各 26 行）。
7. 框坐标映射（416 空间 → 左目 1280×1080）：`x_l = x*1280/416`；`y_l = 60 + (y-52)*960/312`；越界截断。画框用 `phys_addr + ot_smr_mmap`（USER 模式 virt_addr 不可靠），Y 平面画彩色矩形轮廓、框内 UV 填 128。
8. 资源路由：yolo 模型(~8.6MB)目标持久路径 `/data/model/tiny-yolov3_yuv420sp_b.ortm`（重启不丢，兼容 `user_init.sh` 开机自启）；空间不足时按用户授权删除 `/data/npu_demo`（7.5MB 旧 abab 产物，model/bin 已空）等可重编产物。探针阶段的模型/合成输入/输出放 /tmp（tmpfs ~51.9MB，重启即失）；stereo 继续用 `/data/model/stereo_match.ortm.enc`。
9. 推进顺序：先做零代码双模型内存探针；OOM 立即停止汇报，不继续后续里程碑。

## 实现改动与里程碑（每步 ≤200 行、≤5 文件、编译/验证后立即 commit）

### M1 计划落地（本步）

本计划写入 `plan.md` 并提交。

### M2 零代码双模型内存探针（先决门）

- 先 `kill -INT $(pgrep -f stereo_app)` 停板端 stereo_app（勿 kill -9，防 VENC 资源残留）。
- 部署到 `/tmp`：`sample_dual_model_abab`；运行所需 .so 放 `/tmp/lib` 并设 `LD_LIBRARY_PATH=/tmp/lib`；`tiny-yolov3_yuv420sp_b.ortm`（`~/npu_toolchain/common/samples/tiny-yolov3_yuv420sp/`）；stereo 明文模型；合成输入（左/右 640×448 RGB planar 目录 `0`/`1` + 416×416 yuv420sp）。
- `repeat=1`、输出目录 `/tmp/probe_out` 跑一次 ABAB（stereo 走现有 RAW fallback 2 输入目录）。
- 验收：两模型同时加载成功；stereo 为 2in/2out（cost=128×224×320、disp=224×320）；yolo 两路 `[1,H,W,255]` 输出；进程不 OOM；记录加载前后 free/RSS 差值。
- OOM → 停止并汇报实测数字，不进入 M3。
- 提示：/tmp 是 tmpfs（占 RAM），stereo 明文 28MB + 输出 dump ~10MB 会让 RAM 偏紧；若 OOM 可先复跑一次 perf 模式（不带 output_dir）区分「dump 写 tmpfs」与「双模型共驻」的占用，把两组数字一并汇报。

### M3 NPU 异步化 + yolo 模块骨架（分 a/b/c 提交，每步 ≤5 文件）

- a) `stereo_npu.c/h`：改 trigger/wait 分段接口，init 的 `thread_num=2`；`stereo_media.c` npu 线程先改为 trigger(stereo)→wait(stereo)+SubPixel（3 文件）。验证 stereo 原链路 19fps 不退化后提交。
- b) 新增 `stereo_yolo.c/h`：模型加载、IO shape/dtype/stride 运行时查询、416×416 YUV420SP 预处理、解码+NMS 复用 dual_model_abab 的就地解码（置信度 0.6、IoU 0.2；先读 objectness 过阈再读 85 值组；按 tensor stride 就地懒读取，禁整块拷贝；exp 只算一次）；Makefile 加源（3 文件，先可编译不接线）提交。
- c) `stereo_media.c` npu 线程接线：trigger(stereo)→trigger(yolo，仅当有新检测帧)→wait(stereo)+SubPixel→wait(yolo)（1 文件）提交。
- 此步检测帧槽为空时跳过 yolo，验证 stereo 原链路 19fps 不退化。

### M4 检测通道

- 左 VPROC group 开 chn2：`ot_eis_vproc_chn_set_crop` 取原生 960×1280@x=60，270° 旋转缩放为 416×312 YUV420SP、`compress_mode=NONE`、FRC 20→10（几何参数逐行照抄 `dual_model_abab/camera.c`）；attr 竖屏 312×416；独立 buffer pool。
- 新增检测线程维护单槽最新帧（互斥锁、拷贝即消费）；`stereo_yolo_preprocess` 按 camera.c 的 mmap+逐行拷贝+上下各 52 行灰边(128) 填充为 416×416。
- 验收：实测检测通道 ~10fps；/tmp 快照的灰边布局与 camera.c 一致；能稳定输出检测框；stereo 主链路帧率/视差不退化。

### M5 画框 + 端到端

- 检测框从 416 内容域映射到左目 1280×1080：`x_l=x*1280/416`、`y_l=60+(y-52)*960/312`，越界截断。
- `stereo_yolo_draw()` 经 `ot_smr_mmap` 映射左帧 phys：Y 平面画彩色矩形轮廓、框内 UV 填 128；在 npu_proc 推入 venc 队列前调用。
- 验收：`stereo_receiver.py` 原样显示左图带框 + 右图 + 视差，无任何上位机改动。

### M6 资源验证 + 文档

- 5 分钟连续运行记录 free/RSS/CPU（复用 abab skill `assets/board-scripts/` 监控思路），确认无内存单调增长、stereo ~19fps、yolo 10fps。
- README 补用法与实测数据。
- 项目 stereo-dev skill 补「双模型集成」一节（只改项目副本 `sd3589c-stereo-dev/SKILL.md`）。
- 回归 `--raw-only`。

## 测试计划

- M2：两模型加载成功、stereo 2in/2out 尺寸正确、yolo `[1,H,W,255]`、无 OOM；记录 free/RSS 差值。
- M3：stereo 主链路 receiver 19fps±、视差正常；yolo 合成输入输出有效框。
- M4：检测通道 ~10fps、灰边布局与 camera.c 一致、稳定输出检测框；stereo 不退化。
- M5：静态场景左图框位置稳定不闪烁；无新协议帧类型、receiver 日志无异常。
- M6：5min RSS 无单调增长、CPU 合理；raw-only 正常；板端重启后 stereo_app 自启且 yolo 模型仍存在。

## 假设与默认

- yolo 板端模型固定 `~/npu_toolchain/common/samples/tiny-yolov3_yuv420sp/tiny-yolov3_yuv420sp_b.ortm`（416×416 YUV420SP），明文 `_b.ortm` 加载；stereo 继续按模块8加密加载。
- yolo 生产路径 `/data/model/tiny-yolov3_yuv420sp_b.ortm`；放不下则按授权先删 `/data/npu_demo` 旧产物；yolo 缺失时 stereo_app 降级纯 stereo（非致命）。
- 检测 FRC 10fps；画框叠加在 VPROC 输出左帧上，不影响 XY-LUT 矫正坐标与视差测距。
- v1 只画矩形轮廓（UV=128），不画文字标签；yolo 10fps 隔帧画框属预期。

## 交接注意事项（新会话执行前必读）

### 板端现状（2026-08-18 实测）

- stereo_app 仍在运行（PID=1412，RSS≈36.6MB）；任何 NPU/媒体探针前先 `kill -INT $(pgrep -f stereo_app)`，避免 -9。
- 内存 total≈101MB、无 swap；当前 free≈13.8MB / available≈4.9MB（被 stereo_app 占用）；kill 后才有余量。
- 存储：/data 3.2MB、/opt 7.8MB、/tmp 51.9MB(tmpfs)。`/data/npu_demo` 仍占 7.5MB（model/bin 已空、lib 为可重编静态库），yolo 需要持久空间时优先删除它。探针的模型/合成输入/输出放 /tmp。
- 板端资产已就绪并校验：`/opt/stereo/{stereo_app,stereo_calib.json,lut_left.bin,lut_right.bin,license.bin}`、`/data/model/stereo_match.ortm.enc`。
- SSH：192.168.1.101 root/123456，端口 22（9000 是立体流数据端口，不是 SSH 端口）。本机无 sshpass，用 paramiko。

### 构建与源码

- 仓库根 `/home/lzx/lq560v100_sdk`，分支 `feature/stereo-depth`。
- 工具链：`export PATH=/opt/linux/x86-arm/aarch64-otv02-linux-gnu-gcc/bin:$PATH`。
- stereo 编译顺序（stereo clean 会连带删 common 的 .o）：clean stereo_app_bk → clean common → build common → build stereo_app/auth_gen（完整命令见项目 skill「构建与部署」）。
- 模型：stereo 明文 `/home/lzx/lq560v100_sdk/stereo_s_ori_h448_w640_128_sub_v1.7_e300_sim.ortm`；yolo 板端 `/home/lzx/npu_toolchain/common/samples/tiny-yolov3_yuv420sp/tiny-yolov3_yuv420sp_b.ortm`；样例二进制已存在 `main/src/component/npu/samples/dual_model_abab/sample_dual_model_abab`。
- readelf 显示 stereo_app 与 sample_dual_model_abab 仅依赖 libc/libstdc++/libm/ld-linux，无需额外 SDK .so；按计划仍可部署 `/tmp/lib` 兜底。

### Skill 使用与恢复

- 项目强制 skill：`sd3589c-stereo-dev/SKILL.md`；双模型参考：`lq560v100-npu-abab-demo`。
- 动代码前先读 4 份 references：`npu-api-patterns.md` / `board-deploy.md` / `camera-npu-pipeline.md` / `stream-transfer.md`。
- 更新 skill 只改项目副本 `sd3589c-stereo-dev/SKILL.md`；`~/.codex/skills/...` 个人副本仅同步、不优先。
- 若 AI 幻觉写坏 skill：用 git 恢复项目副本（先备份真实新增经验的 diff），例如 `git checkout sd3589c-stereo-dev/SKILL.md`。

### Git 规则（AGENTS.md）

- 每步 ≤200 行、≤5 文件；每完成一个小功能且编译/验证通过立即 commit，信息清晰（Gitflow）。
- 工作区有大量预改 .a 和 untracked 产物；禁 `git add -A`，只提交本任务相关文件。

### 加密规则（模块8，仅按需使用）

- stereo 模型：UID→HMAC-SHA256→AES-CTR-128 加密 + license.bin 设备绑定；本任务 stereo 继续用现成 `/data/model/stereo_match.ortm.enc`，无需重做。
- 新模型若需加密：板端 `/tmp/auth_gen` 取 UID → 上位机 `tools/host/encrypt_model` 加密 → 部署；换设备必须重做。
- yolo 明文 `_b.ortm` 直接加载即可，不加密。

### 关键坑点

- NPU 输出按 stride 就地懒读，禁整块拷贝（实测整块拷贝 ~14.5ms/帧）。
- USER 模式 vproc 帧 virt_addr 不可靠，必须 phys_addr + ot_smr_mmap；chroma 偏移 = phys_addr[1]-phys_addr[0]。
- 检测通道 attr 竖屏 312×416、height 16 对齐；compress_mode=NONE。
- 异常退出后媒体句柄残留：`/opt/ompmod/load_lq560v100 -a` 恢复（无需 reboot）。
- 板端监控脚本从 abab skill `assets/board-scripts/` 原样复制，勿手抄。
