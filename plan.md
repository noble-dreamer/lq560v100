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
