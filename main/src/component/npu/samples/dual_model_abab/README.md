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

| 模块         | 函数                                       | 说明                                                                            |
| ------------ | ------------------------------------------ | ------------------------------------------------------------------------------- |
| 模型初始化   | `model_init()` / `model_prepare_io()`  | 加载模型、查询 IO 信息、分配 tensor、创建 dataset、设置 runtime buffer 和优先级 |
| 模型预处理   | `model_preprocess()`                     | 从 input 目录读取数据并按模型的 shape/dtype/stride 写入输入 tensor              |
| 模型推理     | `ot_avp_npu_trigger()`                   | 异步触发推理                                                                    |
| 模型交替调度 | main 中的 A/B 循环                         | trigger A → trigger B → wait A → wait B                                      |
| 模型后处理   | `model_postprocess()` / `print_topk()` | 输出落盘；分类型 F32`[1,N]` 输出额外打印 top-5                                |
| 资源回收     | `model_destroy()`                        | 销毁 dataset/tensor，卸载模型                                                   |

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
sample_dual_model_abab [modelA] [inputA] [modelB] [inputB] [repeat] [output_dir]
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
- **性能模式**：不传 `output_dir` 时不保存任何输出文件、也不打印 top-k，适合 `repeat=10000` 这类大循环基准测试，避免输出把 `/data` 写满。带上 `output_dir` 时输出命名 `A_frameN_out0.bin` / `B_frameN_out0.bin`，F32 分类输出同时打印 top-5。

## 换成自己的模型要确认的点

1. 模型必须是 `*_b.ortm` 板端离线模型；制作参考 `~/npu_toolchain/common/samples`，用 `oritek_model_gen.sh <dir> board` 生成。
2. 输入文件格式、shape 必须和模型输入一致，否则预处理时用 API 查询到的 shape/stride 拷贝会失败或推理结果无效。
3. 输出解析：非分类模型请替换 `model_postprocess()` 里的 top-k 逻辑（检测模型需加 NMS）。
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

大循环压测时除了延迟，还要同时记录整体 CPU、系统内存和样例进程 CPU，用于判断瓶颈在 CPU 还是内存。下面三个监控脚本和末尾的一键执行脚本均取自仓库根目录 `流程.txt`，仅把进程名从 `ortm_run_model` 改为 `sample_dual_model_abab`。

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

### 七、一键执行：资源监控 + 10000 次 ABAB 推理

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

结果落在 `/data/npu_demo/logs/`：`cpu_usage.log` 是整机 CPU，`mem_usage.log` 是内存与进程 RSS，`proc_cpu.log` 是样例进程 CPU，`abab.log` 是程序运行日志。每个监控脚本把同样的采样行同时写入自己的 `.log` 文件和 `*_console.log`，两个文件都不会为空。测试时注意 `/data` 分区剩余空间（`df -h /data`）；性能模式不写输出文件，10000 次循环只消耗日志空间。
