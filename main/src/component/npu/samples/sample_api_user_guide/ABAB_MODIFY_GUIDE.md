# ABAB 双模型交替调度样例修改说明

本文针对 `sample_api_user_guide` 目录下的 ABAB 模型交替调度实例，解释其工作原理，并说明换成自己的模型时具体要改哪些内容。所有路径均以本目录为当前目录。

## 1. 样例文件与作用

| 文件                             | 作用                                                 |
| -------------------------------- | ---------------------------------------------------- |
| `sample_npu_main.c`            | 入口：初始化 SMR/media，解析用例序号，结束时回收资源 |
| `sample_npu_async_inference.c` | 核心用例：双模型加载 + 交替调度（trigger/wait）      |
| `sample_npu_common.h/.c`       | 通用工具：创建/销毁输入输出 tensor 和 dataset        |
| `Makefile`                     | 编译规则，生成的二进制为`sample_api_user_guide`    |

运行方式：

```text
./sample_api_user_guide 1    # 两个单 graph 模型的异步推理
./sample_api_user_guide 2    # 单 graph 与多 graph 模型混跑
```

## 2. 执行流程

样例当前实现了两种调度方式（都在 `sample_npu_async_inference_both_single_graph` 中）：

```text
用法1: model_0 trigger + model_1 trigger + model_1 wait
用法2: model_0 trigger + model_0 wait + model_1 trigger + model_1 wait
```

底层 API 调用链如下：

```text
ot_avp_npu_init(config)          // config.thread_num = 2 才能使用异步 trigger/wait
  └─ ot_avp_npu_load_model() × 2  // 加载两个 .ortm 离线模型
      └─ 查询输入/输出: num, shape, dtype, stride, size
      └─ ot_avp_npu_malloc() 分配输入输出内存
      └─ ot_avp_npu_create_dataset() / ot_avp_npu_add_buffer()
  └─ ot_avp_npu_trigger() / ot_avp_npu_wait()   // 交替调度
  └─ ot_avp_npu_destroy_dataset() / ot_avp_npu_free()
  └─ ot_avp_npu_unload_model() × 2
ot_avp_npu_deinit()
```

## 3. 换成自己的模型必须修改的项

### 3.1 模型路径（必改）

在 `sample_npu_async_inference.c` 中找到 `model_path[]`：

```c
const char* model_path[] =
    {"../data/model/classification/resnet50_binary_b.ortm",
     "../data/model/classification/resnet50_binary_b.ortm"};
```

改为你自己的板端离线模型路径。注意：

- 板端必须使用 `*_b.ortm`（offline model），不能用仿真模型 `*_s.ortm`。
- 路径相对样例二进制所在的目录，部署到板端后要保持相对目录结构，或用绝对路径。

### 3.2 模型个数（按需改）

```c
const int model_num = 2;
```

如果超过两个模型，同步扩大 `model_info[]` 数组并调整循环。默认样例固定为 2 个。

### 3.3 异步线程数（按需改）

`ot_avp_npu_config` 中的 `thread_num`：

```c
.thread_num = 2,
```

`thread_num > 0` 时同时支持同步和异步推理；`= 0` 只支持同步推理。要使用 trigger/wait 的异步交替调度，必须保持 `thread_num > 0`。

### 3.4 输入数据准备与预处理（重要，现有样例没有做）

现有样例调用 `prepare_input_output_tensor()` 只创建了 tensor 和 dataset，**并没有把输入数据填进去**，因此它只验证了调度流程。要真正跑模型，必须在 trigger 之前为每个输入把数据写入 `model_info[i].input_tensor[j].virt_addr`，即补上预处理模块。

预处理内容取决于模型输入格式，可通过 API 查询：

```c
ot_avp_npu_get_input_num()           // 输入个数
ot_avp_npu_get_input_shape_by_index()   // shape，如 {1,3,224,224}
ot_avp_npu_get_input_dtype_by_index()   // 如 UINT8 / F32 / F16
ot_avp_npu_get_input_format_by_index()  // NCHW / NHWC / ND
ot_avp_npu_get_input_default_stride()   // 行对齐后的跨度
ot_avp_npu_get_input_size_by_index()    // 缓冲区大小
```

三种常见情况的处理方式：

1. 输入是已经按模型要求存好的裸数据（如 SDK 的 `data/ImageNet/binary/*.bin`、`rgbplanar/*.rgb`）：直接读文件到输入 tensor。可复用 `../sample_utils/file_utils.c` 的 `load_data_from_file()`。
2. 输入是 YUV420SP（摄像头原生格式）：参考 `~/npu_toolchain/common/samples/tiny-yolov3_yuv420sp/main.cpp`。若离线模型在转换时就声明 YUV420SP 输入，则直接喂 YUV420SP 数据即可；否则需要自行做 YUV→RGB/NCHW、resize/letterbox 后再写入输入 tensor。
3. 输入是 JPEG/PNG 图片：需要先解码并按模型要求转成目标 dtype、format、shape。

建议为每个模型单独封装一个 `xxx_preprocess()`，在 `trigger` 前调用。

### 3.5 输出解析与后处理（重要，现有样例没有做）

现有样例在 wait 之后没有解析输出。模型跑完后，输出数据位于 `model_info[i].output_tensor[j].virt_addr`，需要按输出信息解析：

```c
ot_avp_npu_get_output_num()
ot_avp_npu_get_output_shape_by_index()
ot_avp_npu_get_output_dtype_by_index()
ot_avp_npu_get_output_default_stride()
```

注意输出缓冲区是按 stride 对齐的，逐行拷贝有效数据时要用 stride，不能直接把整块 buffer 当作紧凑数组。分类模型通常取 top-k；检测模型通常做阈值过滤 + NMS（参考 tiny-yolov3 样例的 `postProcess()`）

### 3.6 模型优先级（trigger+trigger+wait 时必看）

API 文档对 `ot_avp_npu_trigger` 的要求：

- 用法1 `trigger+wait` 与 `trigger+wait`：无额外优先级要求。
- 用法2 `model_0 trigger + model_1 trigger + model_1 wait`：**两个模型优先级必须一致**，否则 `model_1 wait` 返回后不能保证 `model_0` 已执行完。

用下面的接口把两个模型设为同一优先级：

```c
ot_avp_npu_mdl_config cfg = { .priority_level = OT_AVP_MDL_PRI_MEDIUM, .priority_preemp = OT_TRUE };
ot_avp_npu_set_model_config(handle, &cfg);
```

### 3.7 runtime buffer（按模型需要）

部分模型有中间临时内存需求，参考 `ortm_run_model/main.c` 在 load 后处理：

```c
ot_avp_npu_get_runtime_buffer_size(handle, &size);
ot_avp_npu_malloc(&buf, size);
ot_avp_npu_set_runtime_buffer(handle, buf, size);
```

### 3.8 核绑定

`ot_avp_npu_bind_model_to_core()` 在当前版本不支持，不要依赖它做双模型并发绑定。

## 4. 最小改动示例：换成两个不同的 SDK 自带模型

把 A/B 分别换成二进制输入和 RGB-planar 输入的两个模型，能直观看到“不同模型、不同输入格式”的处理：

```c
const char* model_path[] =
    {"../data/model/classification/resnet50_binary_b.ortm",
     "../data/model/classification/mobilenetv2_rgbplanar_b.ortm"};

const char* input_path[] =
    {"../data/ImageNet/binary/ILSVRC2012_val_00024327.bin",
     "../data/ImageNet/rgbplanar/ILSVRC2012_val_00024327.rgb"};

for (int i = 0; i < model_num; i++) {
    // 1. load + prepare_input_output_tensor 不变
    // 2. trigger 前补上输入加载（预处理）
    load_data_from_file(input_path[i],
                        (ot_u8 *)model_info[i].input_tensor[0].virt_addr,
                        model_info[i].input_tensor[0].shape,
                        model_info[i].input_tensor[0].stride.dims[0],
                        model_info[i].input_tensor[0].dtype);
}
```

对应的完整可运行实现见新增样例 `../dual_model_abab/`，它实现了“trigger A → trigger B → wait A → 后处理 A → wait B → 后处理 B”的流水线，并把预处理/推理/后处理拆成独立模块。

## 5. 离线模型制作要点

- 参考工程：`~/npu_toolchain/common/samples/`。
- 转换命令：`./oritek_model_gen.sh <sample_dir> board`，输出 `*_b.ortm`。
- 需要每个样例目录内有 `.onnx` 模型和 `quant_cfg.yml` 量化配置。
- 板端模型限制：npu graph 的 node_name 长度 < 32、node_type 长度 < 16，且模型需包含无权重模型分区（见 API 文档 `ot_avp_npu_config` 注意事项）。

## 6. 板端部署与运行

把编译出的可执行程序、`*_b.ortm` 模型、input 文件上传到板端后运行：

```sh
cd /data/npu_demo
export LD_LIBRARY_PATH=/data/npu_demo/sdk_lib:$LD_LIBRARY_PATH
./bin/sample_dual_model_abab \
  /data/npu_demo/model/resnet50_binary_b.ortm \
  /data/npu_demo/input/ILSVRC2012_val_00024327.bin \
  /data/npu_demo/model/mobilenetv2_rgbplanar_b.ortm \
  /data/npu_demo/input/ILSVRC2012_val_00024327.rgb
```

详细的上传、资源监控脚本流程见仓库根目录的 `流程.txt`。
