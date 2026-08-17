# dual_model_abab：双模型 ABAB 异步流水线样例

本样例演示两个板端离线模型（`*_b.ortm`）按下面的顺序交替调度：

```text
预处理 A -> 预处理 B
  -> trigger A -> trigger B
  -> wait A -> 后处理 A
  -> wait B -> 后处理 B
```

输入来自本地 input 目录/文件，不使用摄像头；这是要部署到板端运行的离线模型流程，不是仿真流程。

## 代码模块

| 模块         | 函数                                                                                                                   | 说明                                                                                               |
| ------------ | ---------------------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------- |
| 模型初始化   | `model_init()` / `model_prepare_io()`                                                                              | 加载模型、查询 IO 信息、分配 tensor、创建 dataset、设置 runtime buffer 和优先级                    |
| 模型预处理   | `model_preprocess()` / `mobilenetv2_rgbplanar_preprocess()` / `tiny_yolov3_yuv420sp_preprocess()`                | trigger 前按模型类型分发：RGB planar 按行带 stride 读入，YUV420SP 整块读入，其余退回通用裸数据加载 |
| 模型推理     | `ot_avp_npu_trigger()`                                                                                               | 异步触发推理                                                                                       |
| 模型交替调度 | main 中的 A/B 循环                                                                                                     | trigger A → trigger B → wait A → wait B                                                         |
| 模型后处理   | `model_postprocess()` / `classification_postprocess()` / `print_topk()` / `tiny_yolov3_yuv420sp_postprocess()` | wait 后按模型类型分发：分类输出落盘并打印 top-5，检测输出落盘并做阈值过滤+NMS                      |
| 数据传输     | `transfer.c` / `transfer_io.c` / `transfer_zlib.c` / `stream_send_*()`                                         | 走 SSH stdio 的成帧流：序列号同步、magic 重同步、CRC 校验、zlib 压缩、stdin 控制帧接收             |
| 资源回收     | `model_destroy()`                                                                                                    | 销毁 dataset/tensor，卸载模型                                                                      |

## 预处理/后处理的挂载点与触发顺序

所有改动都在 `main.c`。每个 frame 的执行顺序如下：

```c
/* 1. trigger 之前：预处理把数据写入 input_tensor[].virt_addr */
model_preprocess(&models[0], input_a);
model_preprocess(&models[1], input_b);

/* 2. 触发两个模型 */
ot_avp_npu_trigger(models[0].handle, ...);
ot_avp_npu_trigger(models[1].handle, ...);

/* 3. wait 之后：后处理读取 output_tensor[].virt_addr */
ot_avp_npu_wait(models[0].handle, -1);
model_postprocess(&models[0], ...);
ot_avp_npu_wait(models[1].handle, -1);
model_postprocess(&models[1], ...);
```

`model_init()` 末尾调用 `model_detect_kind()`，用 API 查询到的输出信息自动区分模型：两路 F32 `[1,H,W,255]` 输出识别为 tiny-yolov3 检测模型，单路 F32 `[1,N]` 输出识别为分类模型，其余按原始数据模型处理。因此不用改 CLI，把 mobilenetv2 和 tiny-yolov3 的路径传进来就会走各自的专用函数。

- `mobilenetv2_rgbplanar_preprocess()`：校验输入为 UINT8 NCHW `[1,3,H,W]`，用 `load_data_from_file()` 按查询到的 stride 逐行写入；resize/crop/mean/std 已固化在离线模型的 Preprocess 节点里，软件侧无需重复做。
- `tiny_yolov3_yuv420sp_preprocess()`：模型转换时已声明 YUV420SP 输入（内部完成 YUV420SP→RGBPlanar + letterbox），直接把整块 YUV420SP 数据 `fread` 进输入 tensor。
- `tiny_yolov3_yuv420sp_postprocess()`：**直接在 stride 对齐的 `[1,H,W,255]` F32 输出上解码**——每个 anchor 先只读 objectness，过阈值才读该 anchor 的 85 个 float，避免把整块 NPU 输出拷进紧凑缓冲（板端实测整块拷贝约 14.5ms/frame，是主要瓶颈）。随后按参考样例 `postProcess()` 做 sigmoid/exp 解码、0.6 目标置信度过滤、按分数降序做 0.2 IoU 的 NMS，最后打印 `class/score/box`。坐标是 416×416 模型输入空间，映射回原图分辨率需要原图宽高，可仿照参考样例的 `rescale()` 补上。

性能模式（不传 `output_dir`）不落盘、不打印 top-5/检测框，但预处理、trigger/wait 以及检测解码+NMS 计算仍完整执行，保证基准测试覆盖真实 CPU 开销且不会写满 `/data`。

板端实测（mobilenetv2_rgbplanar + tiny-yolov3_yuv420sp，10000 帧，性能模式）：约 63s；进程 CPU 平均约为单核的 37%；RSS 稳定约 4.2MB。对比只有 trigger/wait、跳过检测后处理的基线（约 56s、单核 30%、RSS 4.3MB），加入预处理和检测后处理后仍以 NPU 推理为主，CPU 与内存都不是瓶颈。

## 编译

在 `main/src/component/npu/samples` 目录：

```sh
make dual_model_abab
```

或在样例目录直接：

```sh
cd main/src/component/npu/samples/dual_model_abab
make
```

生成 `sample_dual_model_abab`。

## 运行

命令格式（参数都可选，`output_dir` 尤其可选）：

```text
sample_dual_model_abab [modelA] [inputA] [modelB] [inputB] [repeat] [output_dir] [stream]
```

默认值：

```text
A: ../data/model/classification/resnet50_binary_b.ortm
   <- ../data/ImageNet/binary/ILSVRC2012_val_00024327.bin
B: ../data/model/classification/mobilenetv2_rgbplanar_b.ortm
   <- ../data/ImageNet/rgbplanar/ILSVRC2012_val_00024327.rgb
```

在样例目录（`data/` 的相对路径基于该目录）直接运行：

```sh
./sample_dual_model_abab
```

换成自己的模型：

```sh
./sample_dual_model_abab \
  /data/npu_demo/model/my_model_a_b.ortm \
  /data/npu_demo/input/a_input.bin \
  /data/npu_demo/model/my_model_b_b.ortm \
  /data/npu_demo/input/b_input.yuv420sp \
  1000 /data/npu_demo/perf_out
```

- 单输入模型：`inputX` 传输入文件路径。
- 多输入模型：`inputX` 传目录，目录内文件命名为 `0`、`1`、`2`…（对应输入索引）。
- YUV420SP 输入：模型在工具链转换时声明 YUV420SP 输入即可直接喂原始 YUV420SP 文件；需要 RGB/NCHW 时要在此处补转换代码。
- **性能模式**：不传 `output_dir` 时不保存任何输出文件、也不打印 top-k/检测框，适合 `repeat=10000` 这类大循环基准测试，避免输出把 `/data` 写满。带上 `output_dir` 时输出命名 `A_frameN_out0.bin` / `B_frameN_out0.bin`，F32 分类输出同时打印 top-5，tiny-yolov3 同时打印阈值过滤+NMS 后的检测框。
- **stream 模式**：第 8 个参数传 `1`（只发结果）或 `2`（同时发输出 tensor）。此时不写 `/data`，stdout 只承载协议帧，日志自动改走 stderr。见下一节。

## SSH 数据流传输（stream 模式）

之前把后处理结果写到 `/data` 分区时容易把磁盘写满（`/data` 约 35MB，tiny-yolov3 两路输出约 861KB/帧）。本模块改为**复用已有的 SSH 通道**把结果实时传回主机，不占用 `/data`；虚拟 USB（RNDIS）继续留给 upgrade 指令等待，不动它。

### 通道与方向

- 板端样例在 SSH 会话内运行：**stdout 只写协议帧**（板端→主机），**stdin 收主机控制帧**（主机→板端，支持 STOP）。SSH 本身提供传输、加密与认证。
- 主机端用 `host/npu_stream_receiver.py`（Python3，只用标准库）解码流：magic 重同步、CRC 校验、zlib 解压、按 frame 序号查丢帧，并把结果/原始 tensor 落到主机目录。

### 板端→主机结果流

```sh
ssh root@$BOARD_IP 'cd /data/npu_demo && ./bin/sample_dual_model_abab \
  /data/npu_demo/model/mobilenetv2_rgbplanar_b.ortm \
  /data/npu_demo/input/ILSVRC2012_val_00024327.rgb \
  /data/npu_demo/model/tiny-yolov3_yuv420sp_b.ortm \
  /data/npu_demo/input/COCO_val2014_000000568213.yuv420sp \
  1000 /tmp 1' | python3 host/npu_stream_receiver.py -o ./board_out
```

`stream=1` 每帧发 2 条 RESULT 帧（A 的 top-5 / B 的 NMS 后检测框），`stream=2` 再追加 TENSOR 帧（按行紧凑、zlib 压缩后的原始输出）。主机端产物：

```text
board_out/stream.json     流元信息（协议版本、模型名/类型、总帧数）
board_out/results.jsonl   逐帧结果（JSON Lines，方便后续解析）
board_out/*_frame*.bin    stream=2 时的原始 tensor（与落盘模式同名同内容）
```

### 数据同步与协议

每帧固定 32 字节头：`magic("NPST") | version | flags | type | model_id | seq | timestamp_us | orig_len | payload_len | crc32`。`seq` 为 frame 序号、`model_id` 区分 A/B，主机据此重组成帧；流首的 SYNC 帧携带协议版本、模型名/类型和总帧数，流中若混入日志行，接收端按 magic 自动重同步；每帧 payload 带 CRC32（与 `binascii.crc32` 同实现），压缩帧置 `flags.compressed` 后由主机 zlib 解压。RESULT 帧的 payload 以 `kind | count | duration_us` 开头：`timestamp_us` 是发送时刻的板端墙上时钟，`duration_us` 是本帧从循环开始到该模型结果发出所耗的时间，主机端 `results.jsonl` 会同时给出 `duration_ms`。

### 为什么推荐 SSH 而不是虚拟 USB

虚拟 USB/RNDIS 链路当前被 `upgrade` 指令等待占用，复用它会和升级握手抢通道；SSH 是现成的、已验证的连接方式（部署、传日志本来就用它），零新增端口、零板端常驻服务，断开 SSH 即停止传输。

## 换成自己的模型要确认的点

1. 模型必须是 `*_b.ortm` 板端离线模型；制作参考 `~/npu_toolchain/common/samples`，用 `oritek_model_gen.sh <dir> board` 生成。
2. 输入文件格式、shape 必须和模型输入一致，否则预处理时用 API 查询到的 shape/stride 拷贝会失败或推理结果无效。
3. 输出解析：分类模型走 `classification_postprocess()` 的 top-k；tiny-yolov3 检测模型走 `tiny_yolov3_yuv420sp_postprocess()` 的阈值过滤+NMS；其他检测模型请按同样的方式在 `model_postprocess()` 里补一个专用分支。
4. 两个模型优先级默认都设为 MEDIUM；若要改成不同优先级，注意 `trigger+trigger+wait` 组合要求优先级一致。

## 板端部署

按仓库根目录 `流程.txt` 的步骤，将可执行程序、模型、输入上传到 `/data/npu_demo`：

```sh
scp -P $BOARD_PORT ./sample_dual_model_abab $BOARD_USER@$BOARD_IP:/data/npu_demo/bin/
scp -P $BOARD_PORT my_model_a_b.ortm $BOARD_USER@$BOARD_IP:/data/npu_demo/model/
scp -P $BOARD_PORT my_model_b_b.ortm $BOARD_USER@$BOARD_IP:/data/npu_demo/model/
scp -P $BOARD_PORT a_input.bin $BOARD_USER@$BOARD_IP:/data/npu_demo/input/
scp -P $BOARD_PORT b_input.yuv420sp $BOARD_USER@$BOARD_IP:/data/npu_demo/input/
```

板端执行：

```sh
cd /data/npu_demo
export LD_LIBRARY_PATH=/data/npu_demo/lib:$LD_LIBRARY_PATH
./bin/sample_dual_model_abab \
  /data/npu_demo/model/mobilenetv2_rgbplanar_b.ortm \
  /data/npu_demo/input/ILSVRC2012_val_00024327.rgb \
  /data/npu_demo/model/tiny-yolov3_yuv420sp_b.ortm \
  /data/npu_demo/input/COCO_val2014_000000568213.yuv420sp \
  10000 | tee /data/npu_demo/logs/abab.log
```

不传最后的 `output_dir`，即性能模式：只做预处理 → A/B 交替推理，不落盘、不打印 top-k。

## 资源瓶颈评估

大循环压测时除了延迟，还要同时记录整体 CPU、系统内存和样例进程 CPU，用于判断瓶颈在 CPU 还是内存。下面三个监控脚本取自仓库根目录 `流程.txt`，仅把进程名从 `ortm_run_model` 改为 `sample_dual_model_abab`。两档评估分别对应「预处理 + 后处理」（性能模式，见第七节）和「预处理 + 后处理 + 流式传输」（stream=1，见第八节）。

在板端先创建脚本目录：

```sh
mkdir -p /data/npu_demo/scripts /data/npu_demo/logs
```

### 6.1 整机 CPU 占用监控 `/data/npu_demo/scripts/cpu_usage.sh`

```sh
#!/bin/sh

LOG=/data/npu_demo/logs/cpu_usage.log
mkdir -p /data/npu_demo/logs

prev_total=0
prev_idle=0

echo "timestamp,cpu_usage_percent" > $LOG

while true; do
    read -r _ user nice system idle iowait irq softirq steal rest < /proc/stat

    total=$((user+nice+system+idle+iowait+irq+softirq+steal))
    idle_all=$((idle+iowait))

    if [ "$prev_total" -gt 0 ]; then
        dt=$((total-prev_total))
        di=$((idle_all-prev_idle))

        if [ "$dt" -gt 0 ]; then
            usage=$((100*(dt-di)/dt))
        else
            usage=0
        fi

        line="$(date +%s),$usage"
        echo "$line" >> $LOG
        echo "$line"
    fi

    prev_total=$total
    prev_idle=$idle_all

    sleep 1
done
```

### 6.2 内存监控 `/data/npu_demo/scripts/mem_monitor.sh`

```sh
#!/bin/sh

LOG=/data/npu_demo/logs/mem_usage.log
mkdir -p /data/npu_demo/logs

PID=$(ps w | grep sample_dual_model_abab | grep -v grep | awk '{print $1}' | head -n1)

while [ -z "$PID" ]; do
    sleep 1
    PID=$(ps w | grep sample_dual_model_abab | grep -v grep | awk '{print $1}' | head -n1)
done

echo "monitor sample_dual_model_abab PID=$PID"
echo "timestamp,mem_total_kb,mem_free_kb,mem_available_kb,pid_rss_kb" > $LOG

while [ -d /proc/$PID ]; do
    total=$(awk '/MemTotal/{print $2}' /proc/meminfo)
    free=$(awk '/MemFree/{print $2}' /proc/meminfo)
    avail=$(awk '/MemAvailable/{print $2}' /proc/meminfo)
    rss=$(awk '/VmRSS/{print $2}' /proc/$PID/status 2>/dev/null)

    line="$(date +%s),$total,$free,$avail,$rss"
    echo "$line" >> $LOG
    echo "$line"

    sleep 1
done

echo "sample_dual_model_abab process finished"
```

### 6.3 样例进程 CPU 监控 `/data/npu_demo/scripts/proc_cpu.sh`

```sh
#!/bin/sh

LOG=/data/npu_demo/logs/proc_cpu.log
mkdir -p /data/npu_demo/logs

PID=$(ps w | grep sample_dual_model_abab | grep -v grep | awk '{print $1}' | head -n1)

while [ -z "$PID" ]; do
    sleep 1
    PID=$(ps w | grep sample_dual_model_abab | grep -v grep | awk '{print $1}' | head -n1)
done

echo "monitor sample_dual_model_abab PID=$PID"

HZ=$(getconf CLK_TCK 2>/dev/null)
if [ -z "$HZ" ]; then
    HZ=100
fi

prev=0

echo "timestamp,pid,process_cpu_percent_approx" > $LOG

while [ -d /proc/$PID ]; do
    u1=$(awk '{print $14}' /proc/$PID/stat 2>/dev/null)
    s1=$(awk '{print $15}' /proc/$PID/stat 2>/dev/null)

    sleep 1

    u2=$(awk '{print $14}' /proc/$PID/stat 2>/dev/null)
    s2=$(awk '{print $15}' /proc/$PID/stat 2>/dev/null)

    if [ -z "$u2" ] || [ -z "$s2" ]; then
        break
    fi

    cur=$((u2+s2))
    if [ "$prev" -gt 0 ]; then
        delta=$((cur-prev))
        cpu=$((delta*100/HZ))
        line="$(date +%s),$PID,$cpu"
        echo "$line" >> $LOG
        echo "$line"
    fi
    prev=$cur
done

echo "sample_dual_model_abab process finished"
```

三个脚本都赋可执行权限：

```sh
chmod +x /data/npu_demo/scripts/*.sh
```

### 七、评估 1：预处理 + 后处理（性能模式，10000 次）

在板端一键执行：资源监控 + 10000 次 ABAB 推理。性能模式不做流式传输、不落盘、不打印 top-k/检测框，但预处理和检测解码+NMS 仍完整执行，测的是真实 CPU 开销。

```sh
cd /data/npu_demo

if [ -d /data/npu_demo/lib ]; then
    export LD_LIBRARY_PATH=/data/npu_demo/lib:$LD_LIBRARY_PATH
fi

mkdir -p logs

./scripts/cpu_usage.sh > logs/cpu_usage_console.log 2>&1 &
CPU_MON=$!
./scripts/mem_monitor.sh > logs/mem_console.log 2>&1 &
MEM_MON=$!
./scripts/proc_cpu.sh > logs/proc_cpu_console.log 2>&1 &
PROC_MON=$!

echo "CPU_MON PID: $CPU_MON"
echo "MEM_MON PID: $MEM_MON"
echo "PROC_MON PID: $PROC_MON"

sleep 1

./bin/sample_dual_model_abab \
  /data/npu_demo/model/mobilenetv2_rgbplanar_b.ortm \
  /data/npu_demo/input/ILSVRC2012_val_00024327.rgb \
  /data/npu_demo/model/tiny-yolov3_yuv420sp_b.ortm \
  /data/npu_demo/input/COCO_val2014_000000568213.yuv420sp \
  10000 | tee /data/npu_demo/logs/abab.log

kill $CPU_MON $MEM_MON $PROC_MON 2>/dev/null

echo "benchmark finished"
```

跑完后用下面三条命令汇总（`avg` 为均值，`max` 为峰值）：

```sh
cd /data/npu_demo
awk -F, 'NR>1{s+=$2;n++} END{if(n) printf "whole-CPU avg=%.1f%% n=%d\n",s/n,n; else print "whole-CPU: no samples"}' logs/cpu_usage.log
awk -F, 'NR>1{s+=$5;n++} END{if(n) printf "RSS avg=%.0fKB n=%d\n",s/n,n; else print "RSS: no samples"}' logs/mem_usage.log
awk -F, 'NR>1{s+=$3;n++} END{if(n) printf "proc-CPU avg=%.1f%% n=%d\n",s/n,n; else print "proc-CPU: no samples (run too short, needs >=3s)"}' logs/proc_cpu.log
```

结果落在 `/data/npu_demo/logs/`：`cpu_usage.log` 是整机 CPU，`mem_usage.log` 是内存与进程 RSS，`proc_cpu.log` 是样例进程 CPU，`abab.log` 是程序运行日志。每个监控脚本把同样的采样行同时写入自己的 `.log` 文件和 `*_console.log`，两个文件都不会为空。测试时注意 `/data` 分区剩余空间（`df -h /data`）；性能模式不写输出文件，10000 次循环只消耗日志空间。

板端实测参考（mobilenetv2_rgbplanar + tiny-yolov3_yuv420sp，10000 帧）：约 64s；进程 CPU 平均约单核 38%；RSS 稳定约 4.5MB。

### 八、评估 2：预处理 + 后处理 + 流式传输（stream=1，10000 次）

这一档在**主机端**跑：推理结果经 SSH 流回主机，板端不落盘，主机接收器同时校验接收质量。把下面脚本存成文件（例如 `host/benchmark_stream.sh`），在样例目录 `dual_model_abab/` 下执行（脚本里的 `host/npu_stream_receiver.py` 相对路径依赖这个目录），按需改 `BOARD_IP`、`REPEAT`。存好后先加可执行权限，或直接用解释器执行；脚本内部已经以 `root` 账号 ssh 板端，主机侧不需要 `sudo`：

```sh
chmod +x host/benchmark_stream.sh
./host/benchmark_stream.sh      # 或 sh host/benchmark_stream.sh
```

脚本内容：

```sh
#!/bin/sh
BOARD_IP=192.168.1.101
BOARD_USER=root
REPEAT=10000
OUTDIR=./stream_bench_out
HOSTDIR=./stream_bench_logs

mkdir -p "$OUTDIR" "$HOSTDIR"

# 1. 板端起监控（注意先 cd，否则相对路径会落到 /root）
ssh -T $BOARD_USER@$BOARD_IP "cd /data/npu_demo && mkdir -p logs \
  && (nohup ./scripts/cpu_usage.sh >logs/cpu_console.log 2>&1 &) \
  && (nohup ./scripts/mem_monitor.sh >logs/mem_console.log 2>&1 &) \
  && (nohup ./scripts/proc_cpu.sh >logs/proc_console.log 2>&1 &) \
  && sleep 1 && echo monitors started"

# 2. 跑样例：stdout 只有协议帧，管道进主机接收器；板端日志走 ssh 的 stderr
START=$(date +%s)
ssh -T $BOARD_USER@$BOARD_IP "cd /data/npu_demo \
  && export LD_LIBRARY_PATH=/data/npu_demo/lib \
  && ./bin/sample_dual_model_abab \
       /data/npu_demo/model/mobilenetv2_rgbplanar_b.ortm \
       /data/npu_demo/input/ILSVRC2012_val_00024327.rgb \
       /data/npu_demo/model/tiny-yolov3_yuv420sp_b.ortm \
       /data/npu_demo/input/COCO_val2014_000000568213.yuv420sp \
       $REPEAT /tmp 1" 2>"$HOSTDIR/board_stderr.log" \
  | python3 host/npu_stream_receiver.py -q -o "$OUTDIR" 2>"$HOSTDIR/receiver_stderr.log"
END=$(date +%s)
echo "wall seconds: $((END - START))"

# 3. 把板端监控日志拉回主机
scp $BOARD_USER@$BOARD_IP:/data/npu_demo/logs/cpu_usage.log \
    $BOARD_USER@$BOARD_IP:/data/npu_demo/logs/mem_usage.log \
    $BOARD_USER@$BOARD_IP:/data/npu_demo/logs/proc_cpu.log "$HOSTDIR/"

# 4. 与评估 1 相同的三条汇总
awk -F, 'NR>1{s+=$2;n++} END{if(n) printf "whole-CPU avg=%.1f%% n=%d\n",s/n,n; else print "whole-CPU: no samples"}' "$HOSTDIR/cpu_usage.log"
awk -F, 'NR>1{s+=$5;n++} END{if(n) printf "RSS avg=%.0fKB n=%d\n",s/n,n; else print "RSS: no samples"}' "$HOSTDIR/mem_usage.log"
awk -F, 'NR>1{s+=$3;n++} END{if(n) printf "proc-CPU avg=%.1f%% n=%d\n",s/n,n; else print "proc-CPU: no samples (run too short, needs >=3s)"}' "$HOSTDIR/proc_cpu.log"

# 5. 主机接收校验：结果行数应等于 2 * REPEAT，且无重同步/丢帧/CRC 错误
echo "results lines: $(wc -l < "$OUTDIR/results.jsonl") (expect $((REPEAT * 2)))"
if grep -qE 'resync|crc error|seq gap' "$HOSTDIR/receiver_stderr.log"; then
    echo "STREAM ERRORS FOUND:"
    grep -E 'resync|crc error|seq gap' "$HOSTDIR/receiver_stderr.log"
else
    echo "stream clean: no resync / crc error / seq gap"
fi

# 6. 清理板端监控残留
ssh -T $BOARD_USER@$BOARD_IP "pkill -f 'scripts/(cpu_usage|mem_monitor|proc_cpu)' 2>/dev/null; true"
```

说明：第 2 步第 8 个参数 `1` 是 stream 级别（`1`=只发结果，`2`=追加压缩后的输出 tensor），`/tmp` 只是 `output_dir` 占位，流模式下被忽略。`receiver_stderr.log` 末尾的 `stream finished: {1: 1, 2: 20000}` 表示 1 个 SYNC + 20000 条 RESULT；任何日志混流、丢帧或校验失败都会在同一个文件里留下 `resync` / `seq gap` / `crc error` 行。

板端实测参考（同模型同输入，10000 帧）：约 70s；进程 CPU 平均约单核 42%；RSS 稳定约 4.5MB；主机端 20000/20000 条结果、0 次重同步、0 次 CRC 错误、0 次丢帧。相比评估 1 多出的约 6s，约 1.4s 是板端 CPU（分类 top-k 读取 NPU 输出），约 4.5s 是每帧 2 个结果包经过 USB RNDIS→Windows→WSL 链路的固有延迟。若第 8 个参数改成 `2`（加发输出 tensor），瓶颈变为板端 zlib 压缩（673KB tensor 实测约 227ms）与链路带宽，10000 帧约需 57 分钟，仅建议短跑验证时使用。

### 九、相机模式（inputB = camera）

把 B 模型（tiny-yolov3 检测）的输入换成 sc132gs 双目相机，A 模型仍走文件：

```sh
cd /data/npu_demo
./bin/sample_dual_model_abab \
  /data/npu_demo/model/mobilenetv2_rgbplanar_b.ortm \
  /data/npu_demo/input/ILSVRC2012_val_00024327.rgb \
  /data/npu_demo/model/tiny-yolov3_yuv420sp_b.ortm \
  camera 3000 none 1
```

`camera.c` 启动与 `uvc_app` 相同的 sc132gs 管线（**不创建/绑定 UVC gadget**，USB 保持 RNDIS+ACM）：左/右 vproc 组输出原生 1280x960 帧，左组 chn2 检测通道做中心裁剪 960x1280@(60,0) + 270° 旋转 + 缩放，输出 416x312 YUV420SP、FRC 30→10。由于 omg 把 YUV420SP 原始输入尺寸钉死在 ONNX 头尺寸（416x416，无法直接转出 640x480 输入），416x312 内容按上下各 52 行灰边（Y=UV=128）补成模型要求的 416x416，检测框解码后再按参考 `rescale()` 映射回 640x480 报告，RESULT 流帧追加 `src_w/src_h`。

采集线程只保留最新一帧（单槽，队列上界 1），推理侧拷贝即消费；因此每帧周期跟随 10fps 相机。启动前需把 `param/sc132gs` 放到运行目录（或软链），运行需 `--dump-frame` 可把一帧 NPU 视角的 416x416 快照写到 `/tmp/camera_frame.yuv420sp`（注意该参数要放在 positional 参数之外，否则会被当成 output_dir）。

板端实测（相机模式，stream=1，3000 帧约 5 分钟）：每帧 `duration_us` 平均 99.9ms（冷启动首帧约 37ms，后续稳定约 100ms，无 >150ms 帧）；整机 CPU 平均 13.1%（峰值 53%）；进程 CPU 平均单核 15.2%；RSS 启动后稳定在约 11.8MB，最后 250s 无单调增长；主机端 6000/6000 条结果、0 次重同步/CRC 错误/丢帧；`/data` 与 `/tmp` 占用在运行前后不变。左右原生帧 ΔPTS 平均 0-1μs（硬件主从同步），远小于 16.6ms 容差。跑相机模式前若 `sample_comm_vi_start_vi` 报 `get dev_handle failed`，说明上一次异常退出残留了 VI 句柄，先执行 `/opt/ompmod/load_lq560v100 -a` 重置媒体栈再重跑。
