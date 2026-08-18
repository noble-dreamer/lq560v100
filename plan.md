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
