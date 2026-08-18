/*
 * 相机源模块：sc132gs 双目管线 + 检测通道，向 NPU 检测模型提供最新帧。
 * 对外只依赖 stdint，模型输入布局约定见 camera.c 头注释。
 */
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 检测通道输出（270° 旋转后的横向尺寸），内容为 4:3，无畸变 */
#define CAMERA_DET_OUT_W  416
#define CAMERA_DET_OUT_H  312

/* tiny-yolov3_yuv420sp 模型输入：416x416 YUV420SP，内容上下补灰边 */
#define CAMERA_NPU_IN_W   416
#define CAMERA_NPU_IN_H   416
#define CAMERA_NPU_IN_LEN (CAMERA_NPU_IN_W * CAMERA_NPU_IN_H * 3 / 2)

/* 相机检测结果报告用的源分辨率（检测框 rescale 目标） */
#define CAMERA_SRC_W      640
#define CAMERA_SRC_H      480

/* 检测通道帧率：传感器 30fps 源，FRC 目标帧率（1..30，默认 10） */
#define CAMERA_FPS_DEFAULT 10
#define CAMERA_FPS_MAX     30

typedef struct {
    uint32_t det_frames;   /* 检测通道累计帧数 */
    uint32_t lr_pairs;     /* 左右 PTS 配对帧数 */
    uint64_t dpts_min_us;  /* 左右 PTS 差值统计（微秒） */
    uint64_t dpts_avg_us;
    uint64_t dpts_max_us;
} camera_stats;

/* 媒体系统初始化 / 反初始化；必须在 NPU 初始化之前调用 camera_init */
int32_t camera_init(void);
void camera_deinit(void);

/* 启动 / 停止 sc132gs 管线（含 scene_auto 与采集线程） */
int32_t camera_start(uint32_t camera_fps);
void camera_stop(void);

/* 把最新检测帧按 416x312 内容 + 上下灰边拷入 416x416 YUV420SP 输入缓冲 */
int32_t camera_copy_latest_to_input(uint8_t *dst, uint32_t dst_len);

int32_t camera_get_stats(camera_stats *stats);

/* 调试：把最新帧（补边后的 NPU 视角 416x416）快照到 path */
int32_t camera_dump_latest(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* __CAMERA_H__ */
