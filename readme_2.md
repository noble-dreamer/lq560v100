# USB、SSH、串口 Shell 与 IMU 数据走向说明

## 1. 结论

这块设备只有一根 USB 线，但这根线可以同时承载多个彼此独立的 USB 功能。启用 RNDIS+ACM 复合设备后，主机通常会同时看到：

- 一块 RNDIS 虚拟网卡：板端对应 `usb0`，用于 IP、TCP 和 SSH；
- 一个 CDC-ACM 虚拟串口：板端对应 `/dev/ttyGS0`，Windows 对应某个 `COMx`，可用于 IMU 数据或串口 Shell。

因此，“使用 SSH”并不会把整根 USB 线独占，也不会让 IMU 数据自动进入 SSH。SSH 数据走 RNDIS/`usb0`，原始 `imu_app` 的 IMU 数据仍走 CDC-ACM/`ttyGS0`。两路数据由 USB 端点和接口编号区分，可以在同一根 USB 线上同时传输。

当前修改后的 SDK 已经禁止启动 `imu_app`，并让 `getty` 使用 `ttyGS0`。所以当前固件中：

- SSH 仍通过 RNDIS 的 `usb0` 通信（前提是启动脚本确实配置了 RNDIS 并启动了 `sshd`）；
- 串口 Shell 通过 CDC-ACM 的 `ttyGS0` 通信；
- IMU 不再由 `imu_app` 持续读取并发送；
- IMU 数据不会自动改走网络，也不会自动保存到文件，而是这一条 IMU 采集/发送链路没有启动。

## 2. 一根 USB 线为什么能同时传 SSH 和串口数据

USB 不是只能承载一种用途的传统串口。一个 USB 设备可以声明为“复合设备”，在同一个物理 USB 连接中提供多个逻辑接口。主机枚举设备后，会给不同接口加载不同驱动。

RNDIS+ACM 模式下的数据路径如下：

```text
                         同一根物理 USB 线
                    ┌────────────────────────┐
                    │                        │
板端 Linux          │  RNDIS 接口/独立端点   │        Windows 或 Linux 主机
                    │                        │
sshd                │                        │   SSH 客户端
  ↕                 │                        │      ↕
TCP/IP              │                        │   TCP/IP
  ↕                 │                        │      ↕
usb0 ───────────────┼────────────────────────┼── RNDIS 虚拟网卡
                    │                        │
                    │  CDC-ACM 接口/独立端点  │
                    │                        │
getty 或 imu_app    │                        │   串口终端或 IMU 接收程序
  ↕                 │                        │      ↕
/dev/ttyGS0 ────────┼────────────────────────┼── COMx 或 /dev/ttyACM0
                    │                        │
                    └────────────────────────┘
```

RNDIS 数据包和 CDC-ACM 数据包具有不同的 USB 接口和端点地址。USB 主控制器会对它们进行调度和复用，主机驱动收到数据后再分别交给网络栈或串口驱动。因此即使 SSH 正在大量通信，应用程序仍然可以通过 `/dev/ttyGS0` 发送数据；它们共享物理带宽，但不是同一个 Linux 文件或同一个上层协议。

一个更准确的描述是：

```text
不是：虚拟串口被改成了虚拟网卡

而是：原来的 USB 功能组合发生了变化
      原始相机模式：UVC + ACM
      SSH 网络模式：RNDIS + ACM
```

ACM 功能仍然存在，所以板端仍会生成 `/dev/ttyGS0`，主机仍会生成一个 COM 端口。RNDIS 是新增加或替代 UVC 的网络功能，板端会生成 `usb0`。

## 3. 什么是 `ttyGS0`

`/dev/ttyGS0` 是 Linux USB Gadget Serial 驱动在“设备端”创建的字符设备：

- `tty` 表示终端类字符设备；
- `GS` 表示 Gadget Serial；
- `0` 表示第 1 个 Gadget Serial 端口。

它不是板上的物理 UART，也不是 RNDIS 网络接口。只有 USB Gadget 配置中创建并绑定了 `acm.usb0` 等串口功能后，系统才会出现 `/dev/ttyGS0`。

两端名称通常对应如下：

| 位置 | CDC-ACM 串口名称 |
|---|---|
| 板端 Linux | `/dev/ttyGS0` |
| Windows 主机 | `COMx`，例如 `COM7` |
| Linux 主机 | 通常是 `/dev/ttyACM0` |

板端程序执行下面的操作时，数据会经过 CDC-ACM 发送给主机的 COM 端口：

```c
open("/dev/ttyGS0", ...);
write(fd, data, length);
```

反方向也是一样：主机向 COM 端口写入的数据，会成为板端从 `/dev/ttyGS0` 读取到的数据。

如果主机没有打开对应 COM 端口，IMU 数据不会因此转入 SSH。它仍然只会被写向 `ttyGS0`；至于是短暂缓冲、阻塞还是被丢弃，取决于 Gadget 串口驱动状态以及 `imu_app` 的写入和错误处理方式。

## 4. 什么是 `getty`

`getty` 是把一个 TTY 设备变成登录终端的程序。它通常负责：

1. 打开指定的 TTY；
2. 设置终端属性；
3. 显示登录提示；
4. 接收用户名；
5. 调用 `login`，认证成功后启动 Shell。

当前脚本中的命令是：

```sh
setsid getty -L 1500000 ttyGS0 vt100 &
```

参数含义如下：

- `setsid`：为 `getty` 创建新的会话，使它可以把该 TTY 作为控制终端；
- `getty`：提供登录终端；
- `-L`：将它作为本地线路使用，不等待传统串口的 modem control 信号；
- `1500000`：配置的终端波特率；
- `ttyGS0`：使用 `/dev/ttyGS0`；
- `vt100`：设置终端类型；
- `&`：在后台运行。

CDC-ACM 的实际数据在 USB 总线上以 USB 传输完成，并不是物理 UART 按每秒 1500000 bit 发送。因此这里的波特率主要用于终端配置和主机软件兼容，不等同于 USB 总线的实际传输速率。

`getty` 需要同时读取命令输入并写出登录提示、Shell 输出。因此，作为交互式 Shell 使用时，`ttyGS0` 应当由一个 `getty` 独占。不能让 `imu_app`、`upgrade_triggerd` 和多个 `getty` 同时无协调地读取同一个 TTY。

## 5. 什么是 RNDIS

RNDIS 是 Remote Network Driver Interface Specification 的缩写。它把 USB 设备呈现为一块网络适配器，而不是串口。

在当前板端网络方案中：

- Gadget Function 名称是 `rndis.usb0`；
- 板端网络接口是 `usb0`；
- 板端地址通常是 `192.168.1.101`；
- Windows 侧通常配置为 `192.168.1.3/24`；
- `sshd` 监听 TCP 22 端口；
- `ssh root@192.168.1.101` 的数据路径是 SSH → TCP → IP → `usb0` → RNDIS → USB。

RNDIS 只负责承载网络帧。它不会扫描 `/dev/ttyGS0`，也不会自动把 IMU 数据转换成 TCP 或 UDP 数据。若需要让 IMU 走网络，必须显式修改或新增板端程序，例如让程序读取 IMU 后连接一个 TCP/UDP 地址，或者在板端开启一个 IMU 网络服务。

## 6. 原始固件中的 IMU 数据走向

原始固件的 `run.sh` 和 `usb_mode_ctl.sh` 会启动：

```sh
./imu_app &
```

`imu_app` 是一个经过 strip 的 AArch64 二进制，SDK 中没有它的可读业务源码。不过从二进制字符串可以确认它包含以下设备路径：

```text
/dev/motionsensor_chip
/dev/motionsensor_mng
/dev/ttyGS0
```

结合原始启动脚本和设备行为，可以确认原始数据路径为：

```text
IMU 硬件
  ↓
运动传感器驱动和设备节点
  ↓
imu_app 持续读取 IMU 样本
  ↓
imu_app 写入 /dev/ttyGS0
  ↓
USB CDC-ACM
  ↓
主机上的 COMx
  ↓
PC 端 IMU 数据接收程序
```

原始 UVC 模式的 `usb-uvc.sh` 创建的是 UVC+ACM 复合设备：UVC 负责相机视频，ACM 负责虚拟串口。原始 `usb_mode_ctl.sh` 还会运行 `upgrade_triggerd`，从同一个 `ttyGS0` 接收升级触发命令。因此这个端口原本更像产品协议端口：

- 板端向主机方向：`imu_app` 输出 IMU 数据；
- 主机向板端方向：`upgrade_triggerd` 接收升级指令。

二进制中没有公开 IMU 帧协议源码，所以仅凭现有 SDK 不能严谨说明每个 IMU 数据帧的字段、字节序和校验算法；这些内容需要协议文档、PC 接收端源码，或对串口数据进行抓取分析。

## 7. 打通 RNDIS+SSH 后，使用 SSH 时 IMU 数据怎么走

如果只是把 USB Gadget 从 UVC+ACM 改成 RNDIS+ACM，同时仍然启动原始 `imu_app`，数据路径如下：

```text
SSH 命令和 Shell 输出：
SSH 客户端 ⇄ RNDIS ⇄ usb0 ⇄ TCP/IP ⇄ sshd ⇄ SSH Shell

IMU 数据：
IMU 硬件 → imu_app → /dev/ttyGS0 → CDC-ACM → 主机 COMx
```

也就是说：

- SSH 终端不会显示 IMU 数据；
- IMU 数据不会包含在 SSH 数据包中；
- 要接收 IMU，PC 端仍需另行打开同一复合设备枚举出的 COM 端口；
- SSH 和 COM 可以同时打开，因为它们分别属于 RNDIS 和 ACM 接口；
- 如果 PC 只打开 SSH、没有打开 COM，IMU 数据也不会转到 SSH 窗口。

这正是“一根 USB 线使用 SSH时，IMU 仍能发往 `ttyGS0`”的原因：应用程序并不是直接占用整根 USB 线，而是分别使用 `usb0` 和 `/dev/ttyGS0`；内核中的 RNDIS Function 与 ACM Function 再将两路数据映射到不同 USB 端点。

## 8. 使用串口 Shell 时，IMU 数据怎么走

这里必须区分两种配置。

### 8.1 同时启动 `getty` 和原始 `imu_app`

这不是一个可靠配置，因为两者都使用同一个 `/dev/ttyGS0`：

```text
imu_app ──写 IMU 字节──┐
                      ├── /dev/ttyGS0 ── CDC-ACM ── PC 串口终端
getty   ──写 Shell 输出┘

PC 命令 ── CDC-ACM ── /dev/ttyGS0 ── getty/其他读取者竞争
```

可能出现的现象包括：

- IMU 二进制数据与登录提示、Shell 输出混在一起；
- 终端出现乱码；
- Shell 命令行被 IMU 数据干扰；
- 多个进程读取同一个 TTY 时，输入可能被不同进程消费；
- 串口 Shell 看似被持续占用或无法正常交互。

因此，启用串口 Shell 后不能继续让原始 `imu_app` 向同一个 `ttyGS0` 持续发送。若必须同时保留两种能力，应选择以下方案之一：

1. 保留 `ttyGS0` 给 Shell，把 IMU 改为 TCP/UDP 网络输出；
2. 在 USB Gadget 中增加第二个 ACM Function，让 Shell 使用 `ttyGS0`、IMU 使用 `ttyGS1`；
3. 设计一个统一的复用协议，由单一守护进程独占 `ttyGS0`，再在应用层区分 Shell 与 IMU 帧。

### 8.2 当前修改后的固件

当前 SDK 中已经注释掉 `imu_app` 的启动，并改为启动 `getty`：

```sh
# ./imu_app &
setsid getty -L 1500000 ttyGS0 vt100 &
```

这时的数据走向是：

```text
PC 串口终端 ⇄ COMx ⇄ CDC-ACM ⇄ /dev/ttyGS0 ⇄ getty ⇄ Shell

IMU 硬件
  └── imu_app 未启动，所以原来的“读取并发往 ttyGS0”链路停止
```

“屏蔽 IMU 发送”不是把 IMU 数据改送到其他位置，而是不启动负责采集并发送的 `imu_app`。除非还有其他进程读取 IMU，否则该程序不再产生或转发这一路 IMU 样本。

## 9. 三种状态对照

| 状态 | USB 功能组合 | SSH 走向 | `ttyGS0` 的使用者 | IMU 走向 |
|---|---|---|---|---|
| 原始 UVC 固件 | UVC+ACM | 无 RNDIS，因此不能经 `usb0` SSH | `imu_app` 输出；`upgrade_triggerd` 接收升级命令 | IMU → `imu_app` → `ttyGS0` → PC COM |
| 打通 SSH、但未屏蔽 IMU | RNDIS+ACM | `usb0` → TCP 22 → `sshd` | `imu_app`；若同时启动 `getty` 会发生冲突 | 仍是 `ttyGS0`，不会自动走 SSH |
| 当前屏蔽 IMU 并启用串口 Shell | UVC+ACM 或 RNDIS+ACM，取决于实际启动脚本 | 只有 RNDIS 模式下才可经 `usb0` SSH | `getty` | `imu_app` 不启动，不再发送 IMU |

## 10. 当前 SDK 与板端临时 RNDIS 配置的区别

当前 SDK 源码的默认 `user_init.sh` 仍然调用：

```sh
/etc/usb_mode_ctl.sh uvc
```

所以仅根据当前 SDK 默认启动链重新编译、烧写后，默认创建的是 UVC+ACM，而不是 RNDIS+ACM。串口 Shell 可以存在，但不代表 RNDIS 和 SSH 一定存在。

之前为了打通板端 SSH，部署到板端的 `usb-ether.sh` 会创建：

```text
functions/rndis.usb0
functions/acm.usb0
```

并配置 `usb0`、启动 `sshd` 和 `getty`。如果完整烧写覆盖了 `/opt/user_init.sh` 或 `/opt/bin/usb-ether.sh`，需要确认 RNDIS 启动逻辑是否仍然存在，否则设备会回到 SDK 默认的 UVC+ACM 模式。

还要保证每个 `ttyGS0` 只运行一个 `getty`。RNDIS 版 `usb-ether.sh` 已经包含：

```sh
pidof getty >/dev/null 2>&1 || setsid getty -L 1500000 ttyGS0 &
```

而当前 `stereo/run.sh` 的独立运行分支也会启动 `getty`。如果将两套启动链直接组合，应保留一个启动位置并禁用另一个，避免两个 `getty` 同时打开 `ttyGS0`。

## 11. 板端检查方法

### 11.1 检查相关进程

```sh
ps | grep -E 'imu_app|getty|sshd|upgrade_triggerd' | grep -v grep
```

当前目标状态应为：

- 有一个 `getty`；
- 有 `sshd`（如果使用 RNDIS+SSH 模式）；
- 没有 `imu_app`；
- 正常 Shell 模式下没有 `upgrade_triggerd`。

### 11.2 检查网络接口

```sh
ifconfig usb0
```

能看到 `usb0` 以及类似 `192.168.1.101` 的地址，才说明 RNDIS 网络接口已经建立。

### 11.3 检查虚拟串口

```sh
ls -l /dev/ttyGS0
```

存在该设备节点说明板端 Gadget Serial 端口已经创建；主机是否成功枚举还需要在 Windows 设备管理器或 PowerShell 中确认对应 COM 设备。

### 11.4 检查 USB Gadget 中配置了哪些功能

RNDIS 模式可检查：

```sh
ls -l /sys/kernel/config/usb_gadget/ether/configs/c.1/
cat /sys/kernel/config/usb_gadget/ether/UDC
```

预期能看到 `rndis.usb0` 和 `acm.usb0` 的链接，并且 `UDC` 文件非空。

UVC 模式可检查：

```sh
ls -l /sys/kernel/config/usb_gadget/camera/configs/c.1/
cat /sys/kernel/config/usb_gadget/camera/UDC
```

预期能看到 UVC Function 和 `acm.usb0`。

## 12. 相关源码位置

- 原始和当前应用启动：`main/src/platform/fs/userfs/base/stereo/run.sh`
- 当前 USB 模式管理：`main/src/platform/fs/rootfs/base/etc/usb_mode_ctl.sh`
- UVC+ACM Gadget 创建：`main/src/platform/fs/userfs/base/stereo/usb-uvc.sh`
- 当前 SDK 默认启动入口：`main/src/platform/fs/userfs/base/user_init.sh`
- RNDIS+ACM 参考脚本：`/home/lzx/.codex/skills/board-network-setup/board-scripts/rndis-mode/usb-ether.sh`
- RNDIS+SSH 参考启动脚本：`/home/lzx/.codex/skills/board-network-setup/board-scripts/rndis-mode/user_init.sh`
- 原始 SDK 备份：`/home/lzx/lq560v100_sdk_origin`

## 13. 最终理解

应当把这根 USB 线理解为一个承载多个逻辑接口的总线：

```text
同一根 USB 线
├── RNDIS → usb0 → IP/TCP → SSH
└── CDC-ACM → ttyGS0 → IMU 数据或串口 Shell（二者不能无协调地共用）
```

原始 `imu_app` 明确选择 `/dev/ttyGS0`，所以无论用户是否正在使用 SSH，它都不会自动改走 `usb0`。当前为了获得稳定串口 Shell，已经停止启动 `imu_app`，把 `ttyGS0` 留给 `getty`。如果未来既要稳定 Shell 又要 IMU 数据，推荐将 IMU 改为网络 TCP/UDP 输出，或者增加第二个 ACM 串口 `ttyGS1`。
