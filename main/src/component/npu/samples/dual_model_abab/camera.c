/*
 * 相机源模块实现。
 *
 * 数据流：SC132GS 双目 1080x1280@30fps -> 左右 vproc 组中心裁剪
 * 960x1280@(60,0) + 270° 旋转 -> chn0 输出左右原生 1280x960 帧；左组
 * chn2 检测通道缩放为 416x312 YUV420SP，FRC 30->10。采集线程持续取流，
 * 单槽缓存最新检测帧（队列上界 1），推理侧按需拷贝。
 *
 * 模型输入是 416x416 YUV420SP（omg 把原始输入尺寸钉死在 ONNX 头尺寸，
 * 无法直接转出 640x480 输入），因此 416x312 内容上下各补 52 行灰边
 * （Y=UV=128），与离线转换时 letterbox 的标定域一致。
 */
#include "camera.h"
#include "sample_comm.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ot_scene.h"
#include "ot_scenecomm.h"

#define CAMERA_SNS_TYPE   SMS_SC132GS_MIPI_1_5M_30FPS_RAW12_DIRECT_SLAVE
#define CAMERA_PIPE_NUM   4
#define CAMERA_DET_CHN    2
#define CAMERA_TOP_PAD    ((CAMERA_NPU_IN_H - CAMERA_DET_OUT_H) / 2)

typedef struct {
    ot_eis_handle vi_pipe_hdl;
    ot_eis_handle vp_grp_hdl;
    ot_bool       run;
    pthread_t     tid;
} camera_scene_ctx;

typedef struct {
    ot_scene_param   scene_param;
    ot_u32           total_pipe;
    camera_scene_ctx norm_ctx[OT_SCENE_PIPE_MAX_NUM];
} camera_scene_auto_ctx;

static camera_scene_auto_ctx gs_scene;

typedef struct {
    sample_vi_cfg     vi_cfg[2];
    sample_vproc_attr vp_cfg[2];
    ot_bool           started;
    ot_bool           get_run;
    pthread_t         get_tid;
    pthread_mutex_t   lock;
    ot_bool           slot_valid;
    ot_eis_img_frame  slot_frame;
    ot_u32            det_frames;
    ot_u32            lr_pairs;
    ot_u64            dpts_sum;
    ot_u64            dpts_min;
    ot_u64            dpts_max;
} camera_ctx;

static camera_ctx gs_ctx;
static ot_bool gs_sys_init = OT_FALSE;
static ot_eis_handle gs_media_pipe_hdl = OT_NULL;

static void *camera_scene_norm_proc(void *p)
{
    ot_s32 ret = OT_SUCCESS;
    camera_scene_ctx *ctx = (camera_scene_ctx *)p;

    ret = ot_scene_static_vi_set_param(ctx->vi_pipe_hdl, ctx->vp_grp_hdl);
    if (ret != OT_SUCCESS) {
        sample_print("camera scene static vi set param failed, ret:0x%x\n", ret);
        goto proc_exit;
    }

    ret = ot_scene_dynamic_normal_init(ctx->vi_pipe_hdl);
    if (ret != OT_SUCCESS) {
        sample_print("camera scene dynamic normal init failed, ret:0x%x\n", ret);
        goto proc_exit;
    }

    while (ctx->run == OT_TRUE) {
        ret = ot_scene_dynamic_normal_set_param(ctx->vi_pipe_hdl, ctx->vp_grp_hdl);
        usleep(100 * 1000);
        if (ret != OT_SUCCESS) {
            printf("camera scene dynamic set param failed, ret:0x%x\n", ret);
            continue;
        }
    }

    ot_scene_dynamic_normal_deinit(ctx->vi_pipe_hdl);

proc_exit:
    ctx->run = OT_FALSE;
    return OT_NULL;
}

ot_s32 camera_scene_auto_start(const char *ini_path,
                               ot_eis_handle vi_pipe_hdl[OT_SCENE_PIPE_MAX_NUM],
                               ot_eis_handle vp_grp_hdl[OT_SCENE_PIPE_MAX_NUM],
                               ot_u32 max_hdl_num)
{
    ot_s32 ret = OT_SUCCESS;
    ot_u32 i = 0;
    camera_scene_auto_ctx *ctx = &gs_scene;

    memset(ctx, 0, sizeof(camera_scene_auto_ctx));

    ret = ot_scene_create_param(ini_path, &ctx->scene_param, NULL);
    if (ret != OT_SUCCESS) {
        sample_print("camera scene create param failed\n");
        return ret;
    }

    ret = ot_scene_init(&ctx->scene_param);
    if (ret != OT_SUCCESS) {
        sample_print("camera scene init failed, ret:0x%x\n", ret);
        return ret;
    }

    for (i = 0; i < max_hdl_num; i++) {
        ctx->norm_ctx[i].vi_pipe_hdl = vi_pipe_hdl[i];
        ctx->norm_ctx[i].vp_grp_hdl = vp_grp_hdl[i];
    }

    for (i = 0; i < max_hdl_num; i++) {
        ctx->norm_ctx[i].run = OT_TRUE;
        ret = pthread_create(&ctx->norm_ctx[i].tid, NULL, camera_scene_norm_proc,
                             (ot_void *)(ctx->norm_ctx + i));
        if (ret != 0) {
            sample_print("camera scene create thread failed, ret=%d\n", ret);
            goto start_norm_proc_failed;
        }
    }

    ctx->total_pipe = max_hdl_num;
    return OT_SUCCESS;

start_norm_proc_failed:
    for (ot_u32 k = 0; k < i; k++) {
        ctx->norm_ctx[k].run = OT_FALSE;
        pthread_join(ctx->norm_ctx[k].tid, NULL);
    }
    ot_scene_deinit();
    return ret;
}

void camera_scene_auto_stop(void)
{
    camera_scene_auto_ctx *ctx = &gs_scene;

    for (ot_u32 i = 0; i < ctx->total_pipe; i++) {
        ctx->norm_ctx[i].run = OT_FALSE;
        pthread_join(ctx->norm_ctx[i].tid, NULL);
    }
    ot_scene_deinit();
}

static ot_s32 camera_get_vrb_cfg(ot_u32 pipe_num, ot_u32 width, ot_u32 height,
                                 ot_video_buffer_attr *buffer_attr)
{
    ot_eis_buf_size_calc_cfg calc_cfg;
    ot_eis_img_attr img_attr;

    if (buffer_attr == OT_NULL) {
        return -1;
    }

    buffer_attr->cnt = 2;

    img_attr.width = width;
    img_attr.height = height;
    img_attr.bit_depth = OT_EIS_PIXEL_BIT_DEPTH_8;
    img_attr.pixel_fmt = OT_EIS_IMAGE_FORMAT_BAYER_12BPP;
    img_attr.compress_mode = OT_EIS_IMAGE_COMPRESS_MODE_NONE;
    img_attr.layout = OT_EIS_IMAGE_LAYOUT_LINEAR;
    sample_common_get_pic_buf_cfg(&img_attr, &calc_cfg);
    buffer_attr->buf_blks[0].size = calc_cfg.blk_size;
    buffer_attr->buf_blks[0].cnt = pipe_num * 3;
    strcpy(buffer_attr->buf_blks[0].region_name, "anony");

    img_attr.pixel_fmt = OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR;
    sample_common_get_pic_buf_cfg(&img_attr, &calc_cfg);
    buffer_attr->buf_blks[1].size = calc_cfg.blk_size;
    buffer_attr->buf_blks[1].cnt = pipe_num * 3;
    strcpy(buffer_attr->buf_blks[1].region_name, "anony");

    return 0;
}

int32_t camera_init(void)
{
    ot_s32 ret;
    ot_eis_img_size img_size = {0};
    ot_video_buffer_attr vrb_cfg;

    sample_comm_vi_get_size_by_sns_type(CAMERA_SNS_TYPE, &img_size);
    camera_get_vrb_cfg(CAMERA_PIPE_NUM, img_size.width, img_size.height, &vrb_cfg);
    ret = sample_comm_sys_init(&vrb_cfg);
    if (ret != OT_SUCCESS) {
        printf("[camera] sample_comm_sys_init fail, ret=%d\n", ret);
        return ret;
    }

    ret = sample_comm_sys_set_vi_vproc_init_cfg(OT_EIS_VI_OFFLINE_VPROC_OFFLINE);
    if (ret != OT_SUCCESS) {
        printf("[camera] set vi/vproc offline fail, ret=%d\n", ret);
        sample_comm_sys_exit();
        return ret;
    }

    ret = sample_comm_media_pipe_init(&gs_media_pipe_hdl);
    if (ret != OT_SUCCESS) {
        printf("[camera] media pipe init fail, ret=%d\n", ret);
        sample_comm_sys_exit();
        return ret;
    }

    ret = ot_buffer_pool_init();
    if (ret != OT_SUCCESS && ret != OT_ERR_BUFPOOL_ALREADY_INIT) {
        printf("[camera] buffer pool init fail, ret=%d\n", ret);
        sample_comm_media_pipe_stop(gs_media_pipe_hdl);
        sample_comm_sys_exit();
        return ret;
    }

    gs_sys_init = OT_TRUE;
    printf("[camera] media system init ok\n");
    return OT_SUCCESS;
}

void camera_deinit(void)
{
    if (gs_sys_init != OT_TRUE) {
        return;
    }
    ot_buffer_pool_deinit();
    sample_comm_media_pipe_stop(gs_media_pipe_hdl);
    sample_comm_sys_exit();
    gs_sys_init = OT_FALSE;
    printf("[camera] media system exit ok\n");
}

static ot_s32 camera_user_pool_create(ot_eis_img_attr *img_attr, ot_eis_handle *pool_hdl)
{
    ot_s32 ret;
    ot_u32 block_size = 0;
    ot_video_buffer_attr pool_attr = {0};
    ot_eis_handle tmp = OT_NULL;

    img_attr->bit_depth = OT_EIS_PIXEL_BIT_DEPTH_8;
    sample_common_get_buffer_pool_cfg(img_attr, &block_size);
    pool_attr.cnt = 1;
    pool_attr.buf_blks[0].cnt = 10;
    pool_attr.buf_blks[0].size = block_size;
    ret = ot_buffer_pool_create(&tmp, &pool_attr);
    if (ret != OT_SUCCESS) {
        printf("[camera] buffer pool create fail, ret=%d\n", ret);
        return ret;
    }
    *pool_hdl = tmp;
    return OT_SUCCESS;
}

int32_t camera_start(void)
{
    ot_s32 ret = OT_SUCCESS;
    ot_s32 i = 0;
    ot_s32 done_vi = 0;
    ot_s32 done_pool0 = 0;
    ot_s32 done_pool2 = 0;
    ot_s32 done_vp = 0;
    ot_s32 done_bind = 0;
    ot_bool pipe_sw[OT_EIS_VPROC_GRP_PIPE_MAX_NUM] = {OT_TRUE, OT_FALSE, OT_FALSE, OT_FALSE};
    ot_bool chnl_sw_l[OT_EIS_VPROC_GRP_CHN_MAX_NUM] = {OT_TRUE, OT_FALSE, OT_TRUE, OT_FALSE};
    ot_bool chnl_sw_r[OT_EIS_VPROC_GRP_CHN_MAX_NUM] = {OT_TRUE, OT_FALSE, OT_FALSE, OT_FALSE};
    ot_eis_handle vi_pipe_hdl[OT_EIS_VI_MAX_PIPE_NUM] = {0};
    ot_eis_handle vp_grp_hdl[OT_EIS_VI_MAX_PIPE_NUM] = {0};

    if (gs_sys_init != OT_TRUE || gs_ctx.started == OT_TRUE) {
        return OT_FAILURE;
    }

    for (i = 0; i < 2; i++) {
        ot_s32 dev_id = (i == 0) ? 0 : 2;

        sample_comm_vi_get_default_vi_cfg_by_dev_id(CAMERA_SNS_TYPE, &gs_ctx.vi_cfg[i], dev_id);
        for (ot_s32 j = 0; j < 2; j++) {
            gs_ctx.vi_cfg[i].pipe_info[0].pipe_id[j] = 2 * i + j;
        }
        ret = sample_comm_vi_start_vi(&gs_ctx.vi_cfg[i]);
        if (ret != OT_SUCCESS) {
            sample_print("camera vi start %d fail, ret:0x%x\n", i, ret);
            goto vi_fail;
        }
        gs_ctx.vi_cfg[i].media_pipe_hdl = gs_media_pipe_hdl;
        done_vi++;
    }
    for (i = 0; i < 2; i++) {
        sample_media_vproc_get_default_attr_by_snsor(CAMERA_SNS_TYPE, &gs_ctx.vp_cfg[i]);
        gs_ctx.vp_cfg[i].chn_attr[0].mode = OT_EIS_VPROC_WORK_MODE_USER;
        gs_ctx.vp_cfg[i].chn_attr[0].frame_queue_depth = VPROC_FRAME_QUEUE_DEPTH;
        gs_ctx.vp_cfg[i].chn_attr[0].image_attr.pixel_fmt = OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR;
        gs_ctx.vp_cfg[i].chn_attr[0].image_attr.width = 960;
        gs_ctx.vp_cfg[i].chn_attr[0].image_attr.height = 1280;
    }
    gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].mode = OT_EIS_VPROC_WORK_MODE_USER;
    gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].frame_queue_depth = 2;
    gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].image_attr.pixel_fmt = OT_EIS_IMAGE_FORMAT_YUV_420_SEMIPLANAR;
    /* 270° 旋转交换输出宽高：按竖版 W=H_out/H=W_out 配置 */
    gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].image_attr.width = CAMERA_DET_OUT_H;
    gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].image_attr.height = CAMERA_DET_OUT_W;
    gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].image_attr.compress_mode = OT_EIS_IMAGE_COMPRESS_MODE_NONE;
    gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].frc.src_frame_rate = 30;
    gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].frc.dst_frame_rate = 10;

    for (i = 0; i < 2; i++) {
        ot_eis_handle pool_hdl = OT_NULL;

        ret = camera_user_pool_create(&gs_ctx.vp_cfg[i].chn_attr[0].image_attr, &pool_hdl);
        if (ret != OT_SUCCESS) {
            goto pool_fail;
        }
        gs_ctx.vp_cfg[i].chn_attr[0].pool_handle = pool_hdl;
        done_pool0++;
    }
    {
        ot_eis_handle pool_hdl = OT_NULL;

        ret = camera_user_pool_create(&gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].image_attr,
                                      &pool_hdl);
        if (ret != OT_SUCCESS) {
            goto pool_fail;
        }
        gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].pool_handle = pool_hdl;
        done_pool2++;
    }
    ret = sample_comm_start_vproc(&gs_ctx.vp_cfg[0], pipe_sw, chnl_sw_l);
    if (ret != OT_SUCCESS) {
        sample_print("camera vproc L start fail\n");
        goto vp_fail;
    }
    done_vp++;
    ret = sample_comm_start_vproc(&gs_ctx.vp_cfg[1], pipe_sw, chnl_sw_r);
    if (ret != OT_SUCCESS) {
        sample_print("camera vproc R start fail\n");
        goto vp_fail;
    }
    done_vp++;

    for (i = 0; i < 2; i++) {
        ot_eis_vproc_chn_rotation ro_param = {0};

        gs_ctx.vp_cfg[i].set_attr.crop_param.enable = OT_TRUE;
        gs_ctx.vp_cfg[i].set_attr.crop_param.crop_type = OT_EIS_COORD_ABS;
        gs_ctx.vp_cfg[i].set_attr.crop_param.crop_rect.x = 60;
        gs_ctx.vp_cfg[i].set_attr.crop_param.crop_rect.y = 0;
        gs_ctx.vp_cfg[i].set_attr.crop_param.crop_rect.width = 960;
        gs_ctx.vp_cfg[i].set_attr.crop_param.crop_rect.height = 1280;
        ot_eis_vproc_grp_set_crop(gs_ctx.vp_cfg[i].grp_hdl,
                                  &gs_ctx.vp_cfg[i].set_attr.crop_param);
        ro_param.enable = OT_TRUE;
        ro_param.angle = OT_EIS_RTT_270;
        ot_eis_vproc_chn_set_rotation(gs_ctx.vp_cfg[i].chn_hdl[0], &ro_param);
        if (i == 0) {
            ot_eis_vproc_chn_set_rotation(gs_ctx.vp_cfg[i].chn_hdl[CAMERA_DET_CHN], &ro_param);
        }
    }
    for (i = 0; i < 2; i++) {
        ret = sample_comm_vi_bind_vproc(gs_ctx.vi_cfg[i].pipe_info[0].chn_info.chn_hdl,
                                        gs_ctx.vp_cfg[i].pipe_hdl[0], gs_media_pipe_hdl);
        if (ret != OT_SUCCESS) {
            sample_print("camera vi bind vproc %d fail, ret:0x%x\n", i, ret);
            goto bind_fail;
        }
        done_bind++;
    }
    for (i = 0; i < 2; i++) {
        vi_pipe_hdl[i] = gs_ctx.vi_cfg[i].pipe_info[0].pipe_hdl;
        vp_grp_hdl[i] = gs_ctx.vp_cfg[i].grp_hdl;
    }
    ret = camera_scene_auto_start("./param/sc132gs", vi_pipe_hdl, vp_grp_hdl, 2);
    if (ret != OT_SUCCESS) {
        sample_print("camera scene auto start fail, ret:0x%x\n", ret);
        goto bind_fail;
    }
    gs_ctx.started = OT_TRUE;
    printf("[camera] pipeline up: chn0=960x1280 YVU420SP, chn2=%dx%d YUV420SP frc=30->10\n",
           CAMERA_DET_OUT_W, CAMERA_DET_OUT_H);
    return OT_SUCCESS;

bind_fail:
    for (i = 0; i < done_bind; i++) {
        sample_comm_vi_un_bind_vproc(gs_ctx.vi_cfg[i].pipe_info[0].chn_info.chn_hdl,
                                     gs_ctx.vp_cfg[i].pipe_hdl[0], gs_media_pipe_hdl);
    }
vp_fail:
    for (i = 0; i < done_vp; i++) {
        sample_comm_stop_vproc(&gs_ctx.vp_cfg[i]);
    }
pool_fail:
    for (i = 0; i < done_pool0; i++) {
        ot_buffer_pool_destroy(gs_ctx.vp_cfg[i].chn_attr[0].pool_handle);
    }
    if (done_pool2 != 0) {
        ot_buffer_pool_destroy(gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].pool_handle);
    }
vi_fail:
    for (i = 0; i < done_vi; i++) {
        sample_comm_vi_stop_vi(&gs_ctx.vi_cfg[i]);
    }
    return ret;
}

void camera_stop(void)
{
    if (gs_ctx.started != OT_TRUE) {
        return;
    }
    if (gs_ctx.get_run == OT_TRUE) {
        gs_ctx.get_run = OT_FALSE;
        pthread_join(gs_ctx.get_tid, NULL);
    }
    camera_scene_auto_stop();
    for (ot_s32 i = 0; i < 2; i++) {
        sample_comm_vi_un_bind_vproc(gs_ctx.vi_cfg[i].pipe_info[0].chn_info.chn_hdl,
                                     gs_ctx.vp_cfg[i].pipe_hdl[0], gs_media_pipe_hdl);
    }
    for (ot_s32 i = 0; i < 2; i++) {
        sample_comm_stop_vproc(&gs_ctx.vp_cfg[i]);
        ot_buffer_pool_destroy(gs_ctx.vp_cfg[i].chn_attr[0].pool_handle);
    }
    ot_buffer_pool_destroy(gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].pool_handle);
    for (ot_s32 i = 0; i < 2; i++) {
        sample_comm_vi_stop_vi(&gs_ctx.vi_cfg[i]);
    }
    gs_ctx.started = OT_FALSE;
    printf("[camera] pipeline stopped\n");
}
