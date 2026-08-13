# 双目相机—IMU 时间戳一致性审查结论

## 审查范围

本结论基于当前工作区已应用的 CP001、CP002、CP003 补丁后的代码，以及 `D:\intern\second` 中交付的 VI / Media Sys 开发参考文档和补丁说明。

待验证指标：

1. 相机图像、IMU 加速度/陀螺仪和主控使用同一系统单调时钟；不得各自使用本地时钟；时间戳统一为 `ns`。
2. 相机与 IMU 时间戳同步误差小于 `50 us`。

## 总结

**当前代码未完成上述指标，也没有可复现的证据证明同步误差小于 `50 us`。**

当前 SDK 内部媒体 PTS 的实际单位为 **微秒（`us`）**，而非 `ns`。UVC 与 IMU 对外传输时又将该 PTS 除以 `1000`、截断为 32 位，因此主控侧得到的是 **毫秒（`ms`）的 32 位值**，不满足统一 `uint64_t ns` 的要求。

## 逐项校验

| 指标 | 结论 | 依据 |
| --- | --- | --- |
| 相机帧内部具备 64 位 PTS | 部分满足 | `ot_video_frame.pts` 为 `ot_u64`。 |
| IMU 内部具备 64 位 PTS | 部分满足 | `ot_msensor_sample_data.pts` 为 `ot_u64`，管理缓冲区也保存 64 位 PTS。 |
| 板端 PTS 统一为 ns | 不满足 | 代码直接把 PTS 差值解释为 `us`。 |
| 对外 UVC 图像时间戳为 64 位 ns | 不满足 | UVC 将 PTS `/1000` 后存入 `ot_u32`，仅写 4 字节。 |
| 对外 IMU 时间戳为 64 位 ns | 不满足 | `imu_app` 将 PTS `/1000` 后截为 32 位，仅发送 4 字节。 |
| 相机、IMU 使用同一单调时钟 | 不满足/无同源证据 | 预编译模块反汇编显示普通系统 PTS 与 VI PTS 读取不同的计数寄存器路径；VI 帧 PTS 又经 ISP 硬件时间戳回调取得。无法由此确认两个硬件计数器是否共用同一振荡源，但代码中没有统一时钟域转换或相位同步。 |
| 时间戳代表真实采样/曝光时刻 | 未证实；IMU 当前实现不满足严格含义 | IMU PTS 在软件读取 FIFO 后按平均间隔插值得出，不是在每个 ADC 采样边沿锁存。相机 PTS 未找到公开定义其对应曝光开始/中点/结束的证据。 |
| 同步误差 < 50 us | 未完成/未验证 | 没有硬件触发、FSYNC 使用、误差统计、50 us 门限或测试报告。 |

## 证据

### 1. 当前 PTS 的单位是 us

- [motionsensor_buf.c](main/src/ext_drv/motionfusion/src/kernel/motionsensor_mng/src/motionsensor_buf.c) 第 692 行将 `end_pts - begin_pts > 5000000` 明确注释为 `5000000us`。
- [sample_vproc.c](main/src/component/media/sample/vproc/sample_vproc.c) 第 1625–1628 行以 `ot_eis_media_get_cur_pts() - frame.pts` 计算处理时延，并输出 `use %lld us`。
- [dis_send.c](main/src/component/media/sample/dis/tools/send/dis_send.c) 第 157–159 行声明 `hmax_times` 为 ns、`read_out_time` 和 `time_delay` 为 us；第 717 行将 `hmax_times * height / 1000` 转换为 us，并在第 1312、1318 行与 `frame_pts` 直接相加。因此 `frame_pts` 与 IMU PTS 处于 us 时基。

**结论：** 当前 `src_frame[0].pts` / `frame_pts` / IMU PTS 在本 SDK 的使用约定中是 `us`。所以 `/1000` 后的值是 `ms`，不是 `ns`。

### 2. 内部结构有 64 位 PTS，但仅是字段与传递能力

- 相机帧 PTS 字段：[`ot_common_video.h`](main/src/component/media/common/src/include/ot_common_video.h) 第 364 行，`ot_u64 pts`。
- 相机帧 PTS 传递：[`media_trans_common_utils.c`](main/src/component/media/common/src/media_trans_common_utils.c) 第 377、443 行。
- IMU 样本 PTS 字段：[`ot_common_motionsensor.h`](main/src/ext_drv/motionfusion/include/ot_common_motionsensor.h) 第 40–46 行，`ot_u64 pts`。
- IMU 环形缓冲区写入完整 PTS：[`motionsensor_buf.c`](main/src/ext_drv/motionfusion/src/kernel/motionsensor_mng/src/motionsensor_buf.c) 第 333 行。

这些证据只能证明板端内部能携带 64 位 PTS，**不能证明单位为 ns、时钟同源，或时间点对应真实传感器事件。**

### 3. UVC 和 IMU 对外协议破坏了时间戳精度与长度

#### UVC 图像

[`uvc_media.c`](main/src/component/media/sample/uvc_app/uvc_media.c) 第 215 行：

```c
ot_u32 ... pts = match_src->src_frame[0].pts / 1000;
```

同文件第 318–321 行仅写入 `pts` 的 4 个字节。测试版本有相同行为：[`uvc_test/uvc_media.c`](main/src/component/media/sample/uvc_test/uvc_media.c) 第 197、313–318 行。

#### IMU 加速度/陀螺仪

[`imu_app/main.c`](main/src/component/media/sample/imu_app/main.c) 第 347 行将输出 PTS 定义为 `uint32_t`；第 360 行读取原始 `ot_u64` PTS；第 363 行执行：

```c
pts = (p_viraddr[j] / 1000) & 0xFFFFFFFF;
```

第 365–368 行仅发送 4 个字节。

**结论：** 板端对外发送的是 `uint32_t ms`，不是 `uint64_t ns`。主控不能依据该字段进行 50 us 级同步。

### 4. 32 位 ms 的回绕问题

当前对外值为 32 位毫秒计数：

```text
2^32 ms = 4,294,967,296 ms ≈ 49.71 天
```

约 49.71 天后数值从 `0xFFFFFFFF` 回到 `0`。当前 `imu_app` 在 [`main.c`](main/src/component/media/sample/imu_app/main.c) 第 381–391 行以 `pts < last_pts` 判定 PTS 非法，因此回绕会被错误报告为时间倒退。

不应通过主控侧猜测回绕次数来满足本需求；应修改板端协议为全链路发送 **无截断的 `uint64_t timestamp_ns`**。从零开始的 64 位 ns 计数约 584 年才回绕。

### 5. IMU PTS 不是每条数据的硬件采样边沿时间

- 驱动默认选用 `TRIGER_TIMER`：[`icm42670.c`](main/src/ext_drv/motionfusion/src/kernel/motionsensor_chip/sensor_dev/icm42670/icm42670.c) 第 78 行。
- hrtimer 使用 `CLOCK_MONOTONIC`：同文件第 2194 行。这是单调时钟的正面证据，但只说明软件定时器基于单调时钟。
- 读取 FIFO 前以 `call_sys_get_time_stamp()` 取得当前系统时间：同文件第 1989、2011 行。
- `time_inter` 通过多个读取周期的时间差除以 FIFO 样本数计算：同文件第 1674–1707 行。
- 每条 gyro/acc PTS 采用 `last_pts + time_inter` 生成：同文件第 1576、1593 行。

因此，IMU PTS 是“软件读 FIFO 时刻锚定后的平均间隔估计”，会受 hrtimer、工作队列、SPI 读取、FIFO 深度和调度延迟影响；它不是每条 IMU ADC 转换瞬间被 SoC 统一时钟硬件锁存的时间。

此外，ICM42670 硬件具备 FSYNC 与芯片时间戳能力，但当前初始化默认关闭 FSYNC，且默认分辨率为 16 us：[`icm42670.c`](main/src/ext_drv/motionfusion/src/kernel/motionsensor_chip/sensor_dev/icm42670/icm42670.c) 第 1432–1436 行。代码中没有将 FSYNC 时间戳转化和锁定到系统统一时钟域的实现。

### 6. 相机与 IMU 同源时钟没有得到证明

系统接口将两类时间戳明确区分：

- `pfn_sys_get_time_stamp()`：IMU 使用；[`sys_ext.h`](main/src/common/src/include/sys_ext.h) 第 343、405–408 行。
- `pfn_sys_get_vi_time_stamp(dev_id)`：VI 使用；同文件第 388–414 行。

CP001 启用了 `CONFIG_OT_VI_PTS`，但未提供上述两个底层函数的源码实现。虽然 `libot_media_sys.so` 为预编译、已剥离库，交付的内核模块 `main/out/ko/ot_sys.ko` 与 `main/out/ko/ot_vi.ko` 保留了足够符号，可作以下静态确认：

- `sys_drv_get_local_pts`（`0x9474`）调用 `sys_hal_get_local_sys_cnt`（`0xa794`），随后除以 24；后者从系统计数寄存器区读取低/高 32 位（模块 `.bss+0x16b8` 所保存的 MMIO 基址，寄存器偏移 `4176/4180`）。
- `sys_drv_get_vi_local_pts`（`0x94a0`）调用 `sys_hal_vi_get_sys_cnt`（`0xa750`），同样除以 24；后者从另一 MMIO 基址（`.bss+0x16c8`）按 `dev_id` 索引读取一对低/高 32 位寄存器。
- `ot_vi.ko` 的 `vi_switch_capture_frame_status`（`0xa864`）调用 `vi_drv_get_pipe_hw_timestamp`（`0x47fc0`）并将返回值写入帧 PTS；该函数从 ISP 模块函数表偏移 `256` 取得回调并调用。因此，实际 VI 帧 PTS 并不是对普通系统 PTS 函数的直接调用。

这证明两类 API **不是同一个软件函数或同一寄存器读取路径**。但模块反汇编无法判定这两组硬件计数器是否由同一个物理振荡器分频；即使共用频率源，仍必须证明 epoch、复位行为、频率、相位关系以及跨域同步误差。当前代码没有发现将 VI/ISP PTS 显式转换、锁定到 `pfn_sys_get_time_stamp()` 时钟域的实现。

#### 三条时间戳链路分别是什么、用于什么

这里的“时钟”应理解为硬件自由运行计数器及其取值链路，并不等于已经确认有三颗独立晶振。

| 链路 | 本质 | 用途 | 与传感器事件的关系 |
| --- | --- | --- | --- |
| `sys_hal_get_local_sys_cnt()` → `pfn_sys_get_time_stamp()` | SoC 通用系统自由运行计数器 | 向媒体模块、IMU 等提供当前系统时间 | 在调用时读取“当前时刻”。本项目的 ICM42670 驱动在软件读取 FIFO 时以此为锚点。 |
| `sys_hal_vi_get_sys_cnt(dev_id)` → `pfn_sys_get_vi_time_stamp(dev_id)` | 按 VI `dev_id` 获取的 VI 专用计数器 | 向 VI 采集链路提供该 VI 时间域的当前时间 | 在调用时读取 VI 域的“当前时刻”；不是普通系统 PTS 函数的直接复用。 |
| `vi_drv_get_pipe_hw_timestamp()` | ISP/VI pipe 导出的硬件时间戳回调 | 在帧状态切换时填入 `frame.pts` | 更接近图像采集链路；但现有代码/二进制未公开说明其锁存点是曝光开始、曝光结束、SOF、EOF、DMA 到达或其他事件。 |

前两条 API 的原始计数都在驱动中除以 `24` 后返回。若原始计数频率为 24 MHz（该除法所表达的常见设计），则：

```text
24,000,000 tick/s ÷ 24 = 1,000,000 单位/s = us
```

即它们返回的是微秒计数，而不是纳秒。相同的 `/24` 换算只能说明二者可能使用相同标称频率，**不能证明**它们共享同一振荡源、相同起点或小于 50 us 的相位误差。

因此当前 IMU PTS 是“软件 FIFO 读取时刻附近的通用系统时间锚点”，而图像帧 PTS 是“ISP/VI pipe 硬件回调给出的帧相关时间”。要证明二者满足同一单调时钟域及 `<50 us`，仍须同时验证两计数域的固定偏移和长期漂移，并用硬件触发（相机帧事件与 IMU FSYNC/采样事件）测量真实事件误差。

交付目录中的《SD3589CV100 VI开发参考》《SD3589CV100 Media Sys开发参考》也未找到可用的公开条款说明 PTS 是 ns、与 IMU 时钟同源，或对应曝光边沿。

因此不能据“二者都名为 PTS”推断它们一定同源；必须由芯片/SDK 供应方提供时钟域、单位和事件语义说明，或通过硬件测试验证。

#### ICM-42670-P 数据手册与当前驱动的进一步结论

本节结合姿态传感器规格书 `D:\intern\C3288646_姿态传感器-陀螺仪_ICM-42670-P_规格书_WJ1549759.PDF` 与当前 SDK 中的 ICM-42670-P 驱动进行确认。

**结论：当前实现中，ICM-42670-P 与相机不是同一个硬件采样时钟源，也没有启用相机帧到 IMU 的硬件 FSYNC 同步。**

这里需要区分三个概念：

| 概念 | 当前状态 |
| --- | --- |
| 相机与 IMU 共用同一个硬件采样时钟 | 否；相机使用 SoC Sensor Clock，IMU 使用芯片内部 RC/PLL。 |
| 相机帧同步脉冲送入 IMU `FSYNC` | PCB 是否连线仅凭 SDK 和 IMU 数据手册无法确认，需要原理图、PCB 网络或示波器测量。 |
| IMU 驱动使用 `FSYNC` 对齐相机帧 | 否；当前驱动初始化时明确关闭 `FSYNC`、FSYNC 时间戳和 FSYNC 中断。 |
| 相机与 IMU PTS 可能使用 SoC 的相关时间基准 | 有可能，但这只是软件时间戳域问题，不等于传感器采样时钟同源，也不能证明误差小于 50 us。 |

##### 6.1 ICM-42670-P 没有相机式外部 MCLK/CLKIN 输入

规格书的引脚表中列出了：

- `FSYNC`：Frame Sync Input，未使用时应接地；
- `INT1/INT2`：中断输出；
- SPI、I2C、I3C 通信引脚；
- 电源及保留引脚。

规格书没有列出可供传感器采样使用的外部 `MCLK` 或 `CLKIN` 引脚。`INTF_CONFIG1.CLKSEL` 的可选项为：

```text
00：始终选择芯片内部 RC oscillator
01：PLL 可用时选择内部 PLL，否则选择内部 RC oscillator（默认）
11：关闭所有时钟
```

因此 ICM-42670-P 的加速度计和陀螺仪 ODR 由芯片内部 RC/PLL 产生，不能把相机的 Sensor Clock 直接作为其采样时钟。

驱动中的 `write_mclk_reg()`、`read_mclk_reg()` 和 `inv_imu_switch_on_mclk()` 不能作为相机与 IMU 共用 MCLK 的证据。这里的 `MCLK` 是 ICM 芯片内部访问 MREG 寄存器所需的内部时钟，不是相机使用的外部 Sensor MCLK。

##### 6.2 相机使用 SoC 提供的 Sensor Clock

当前 UVC 程序配置了两颗 SC132GS：[`uvc_media.c`](main/src/component/media/sample/uvc_app/uvc_media.c) 第 41–50 行：

```c
ctx[i].sns_type  = SMS_SC132GS_MIPI_1_5M_30FPS_RAW12_DIRECT_SLAVE;
ctx[i].sns_num   = 2;
ctx[i].dev_id[0] = 0;
ctx[i].dev_id[1] = 2;
```

相机初始化分别选择 SoC 的 Sensor Clock 0 和 Sensor Clock 1：[`sample_comm_vi.c`](main/src/component/media/sample/common/sample_comm_vi.c) 第 847–859 行：

```c
if (dev_id == 2) {
    sns_info->sns_clk_src = 1;
} else {
    sns_info->sns_clk_src = 0;
}
```

随后由以下接口使能相机时钟：

```c
ot_eis_vi_if_sensor_clk_enable(sns_info->sns_clk_src, OT_TRUE);
```

当前硬件采样时钟关系应理解为：

```text
SoC Sensor Clock 0 ──→ 一颗 SC132GS
SoC Sensor Clock 1 ──→ 另一颗 SC132GS

ICM-42670-P 内部 RC/PLL ──→ IMU 加速度计和陀螺仪 ODR
```

左右相机是否最终由同一上游 PLL 分频，需要 SoC 时钟树资料进一步确认；但无论如何，ICM-42670-P 当前不是通过相机 Sensor Clock 驱动采样。

##### 6.3 ICM-42670-P 支持 FSYNC，但 FSYNC 不是共用采样时钟

ICM-42670-P 的 `FSYNC` 是帧同步事件输入。它可以：

- 标记某次 IMU ODR 数据；
- 测量 FSYNC 边沿到下一次 ODR 的时间差；
- 把 FSYNC 时间信息写入 FIFO；
- 通过中断报告 FSYNC 事件。

这可以建立“哪一批 IMU 样本对应哪一个相机帧事件”的关系，但不会让 IMU 的内部 RC/PLL 与相机像素时钟或帧时钟锁相。因此：

```text
启用 FSYNC = 建立硬件事件对齐/时间关联
共用 MCLK   = 两个传感器由同一个采样频率源驱动
```

二者不是一回事。对当前硬件而言，可实现的方向是使用相机帧事件驱动 ICM 的 `FSYNC`，而不是让 ICM 直接使用相机 MCLK。

##### 6.4 当前 ICM 驱动明确关闭 FSYNC

[`icm42670.c`](main/src/ext_drv/motionfusion/src/kernel/motionsensor_chip/sensor_dev/icm42670/icm42670.c) 第 1403–1424 行的 `inv_imu_disable_fsync()` 明确执行：

```c
value &= ~FSYNC_CONFIG_FSYNC_UI_SEL_MASK;
value |= FSYNC_CONFIG_UI_SEL_NO;

value &= ~TMST_CONFIG1_TMST_FSYNC_EN_MASK;
value |= TMST_CONFIG1_TMST_FSYNC_DIS;
```

初始化函数随后直接调用：

```c
/* Deactivate FSYNC by default */
status |= inv_imu_disable_fsync(s);
```

FSYNC 中断也被关闭：

```c
config_int.INV_UI_FSYNC = INV_IMU_DISABLE;
```

FIFO 配置清除了 `FIFO_CONFIG5_FIFO_TMST_FSYNC_EN_MASK`，代码注释明确表示：

```c
// TMST in the FIFO cannot be replaced by the FSYNC timestamp
```

因此，即便 PCB 已经把某个相机 VSYNC/SOF 信号接到 ICM 的 `FSYNC` 引脚，当前软件也没有消费和保存这个同步事件。

##### 6.5 当前 IMU 时间戳是 SoC 软件时间锚点，不是 FSYNC 锁存时间

ICM 驱动读取 FIFO 时使用：

```c
g_pts_now = call_sys_get_time_stamp();
```

然后依据多次 FIFO 读取的系统时间差和样本数量估算 `time_inter`，为各条 IMU 样本插值生成 PTS。它反映的是 SoC 读取 FIFO 附近的时间基准，而不是 ICM 在每个 ADC 采样边沿锁存的统一硬件时间。

相机帧则携带 `src_frame[0].pts`。两边的数值即使都称为 PTS，也只能说明它们可用于软件关联；在没有证明计数器 epoch、频率、相位和硬件锁存点一致之前，不能称为同一个硬件采样时钟。

##### 6.6 最终判断及仍需补充的硬件证据

当前可以确定：

1. ICM-42670-P 的采样时钟来自其内部 RC/PLL；
2. 两颗 SC132GS 使用 SoC 提供的 Sensor Clock；
3. ICM 没有外部 MCLK/CLKIN 引脚用于接收相机采样时钟；
4. 当前 ICM 驱动关闭了 FSYNC、FSYNC 时间戳和 FSYNC 中断；
5. 因此当前相机与 IMU 既不是采样时钟同源，也没有启用硬件帧同步；
6. 当前软件 PTS 只能提供近似的软件时间关联，不能证明 `<50 us` 同步精度。

仅凭数据手册和 SDK 仍不能确定 PCB 上 ICM 的 7 号 `FSYNC` 引脚实际连接到哪里。需要以下任一证据确认物理连线：

- 相机板和主板原理图；
- PCB Gerber/网络表；
- ICM 7 号引脚及相机 VSYNC/SOF/触发信号的示波器或逻辑分析仪测量；
- 板级 GPIO 复用表与明确的网络名称。

若要实现可靠的相机—IMU 硬件时间关联，应把相机帧事件送到 ICM `FSYNC`，启用 `TMST_FSYNC_EN`、`FIFO_TMST_FSYNC_EN` 和合适的 `FSYNC_CONFIG_UI_SEL`，解析 FIFO 中的 FSYNC 标志及时间差，再把它标定到相机帧 PTS 所在的统一时间域。



### 7. CP001—CP003 没有实现该指标

- CP001 的相关改动是启用 `CONFIG_OT_VI_PTS`，提供 VI PTS 功能开关。
- CP002 的 README 仅描述 gyro 驱动/脚本和 DPU 参数优化。
- CP003 主要管理 UVC、`imu_app` 的启动停止和升级流程。

三个补丁均未包含：64 位 ns 对外协议、相机/IMU 时钟转换、FSYNC 联动、外部触发、同步误差统计或 `< 50 us` 判定。

## 达标所需整改

1. 明确唯一时钟源：定义 SoC 的单调时钟为唯一时间域，并说明其 epoch、单位和溢出行为。
2. 对外协议将相机和 IMU 时间戳字段改为固定 8 字节 `uint64_t timestamp_ns`；取消 `/1000`、`ot_u32` 和掩码截断。
3. 若 SDK 内部 PTS 保持 us，则只在板端做一次无损转换：`timestamp_ns = pts_us * 1000ULL`，并标注来源与语义。不得由主控重新以本机时钟打时间戳。
4. 相机 PTS 必须明确对应曝光开始/中点/结束中的哪一个；姿态解算通常应使用曝光中点，rolling shutter 还需按行时间建模。
5. IMU 应使用外部同步或芯片 FSYNC 时间戳；完成 IMU 芯片时钟与 SoC 单调 ns 时钟的标定，不能仅依赖 FIFO 读取后的平均插值。
6. 为相机触发边沿和 IMU FSYNC 提供同一硬件事件，并保存原始板端日志。

## <50 us 验收测试

软件日志只能检查格式、单调性和漂移，**不能单独证明 50 us**。最终验收应使用脉冲发生器/MCU/FPGA 将同一脉冲扇出到相机外触发与 IMU FSYNC，同时用示波器或逻辑分析仪记录参考边沿。

每次事件记录：

```text
T_cam_ns       相机曝光对应的板端统一时间戳
T_imu_ns       IMU FSYNC/采样对应的板端统一时间戳
offset_ns      经硬件测得的固定链路偏移
residual_ns = (T_cam_ns - T_imu_ns) - offset_ns
```

建议至少采集 10,000 个事件，并覆盖冷启动、持续运行 30 分钟、CPU/DDR/USB 压力和温度变化。通过条件：

```text
max(abs(residual_ns)) < 50,000 ns
```

测试报告应同时交付 p50、p95、p99、最大值、丢帧/丢样数、原始日志、硬件接线和固定偏移标定方法。

# lq560v100
