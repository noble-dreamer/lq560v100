/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <limits.h>
#include <sys/select.h>
#include <sys/time.h>

#include "ot_buffer.h"
#include "ot_common_motionsensor.h"
#include "ot_motionsensor_chip_cmd.h"
#include "ot_motionsensor_mng_cmd.h"
#include "ot_motionfusion.h"
#include "securec.h"
#include "ot_eis_video.h"
#include "ot_eis_vproc_common.h"
#include "ot_eis_vproc_chn.h"
#include "ot_eis_vi.h"
#include "ot_eis_vi_api.h"
#include "ot_eis_common_venc.h"
#include "ot_eis_vproc_grp.h"
#include "ot_eis_venc.h"
#include "ot_eis_media_sys.h"
#include "sample_comm.h"

#define DUMP_FRAME_DEPTH        2
#define FILE_NAME_LENGTH        128
#define MAX_FRM_WIDTH           8192
#define DIS_DUMP_ARG_BASE       10
#define MAX_DIGIT_LEN           4
#define TOTAL_NODE_NUM          8
#define FILE_NAME_SHORT         5
#define DIS_FRAME_BUF_RATIO_MIN 70
#define VENC_FRAME_RATE         30
#define PIC_WIDTH               1936
#define PIC_HEIGHT              1552
#define VENC_BIT_RATE           (1024 * 2 + 2048 * 30 / 30)
#define VENC_BUF_SIZE           (PIC_WIDTH * PIC_HEIGHT * 3 / 2)
#define VENC_STRIDE             64
#define MIN_FRAMENUM             1
#define DUMP_YUV_MODE           0
#define DUMP_STREAM_MODE        1
#define IMU_LUT_LEN             30
#define MAX_VENC_NUM            16

time_t now;
struct tm *timeinfo;
char date_str[20];

static pthread_t g_dis_dump_yuv_pid;
static pthread_t g_dis_dump_stream_pid;
static pthread_t g_dis_dump_yuv_pid1;
static pthread_t g_dis_dump_stream_pid1;

static volatile sig_atomic_t g_signal_flag = 0;
static ot_s32 g_msensor_mngfd = -1;
static ot_s32 g_msensor_userfd = -1;
static ot_u32 g_exposuretime_tmp = 0;
static ot_u32 g_h_max_tmp = 0;
static ot_u32 g_v_max_tmp = 0;
static const ot_u32 fusion_id = 0;
static ot_eis_dis_attr dis_attr;
static ot_eis_dis_cfg dis_cfg;
static FILE *msensor_param_fd;
static FILE *ldc_fd;
static ot_bool g_send_chn_pthread = OT_FALSE;
static ot_bool g_save_file_pthread = OT_FALSE;

typedef enum {
    DIS_DUMP_STREAM = 0,
    DIS_DUMP_IMU_DATA,
    DIS_DUMP_STREAM_AND_IMU_DATA
} dis_dump_mode;

typedef struct {
    ot_u32 exposuretime;
    ot_u32      hmax;
    ot_u32      vmax;
} dis_dump_isp_info;

typedef struct list_exposuretime {
    ot_u64 pts;
    ot_u32 exposuretime;
    ot_u32 h_max;
    ot_u32 v_max;
    struct list_exposuretime* next;
} list_exp;

typedef struct {
    ot_bool enable;
    ot_bool dis_enable;
    ot_vi_pipe vi_pipe;
    ot_s32 chn; //venc_chn_id  vi_chn_id
    ot_eis_handle venc_chn_hdl;
    ot_eis_handle vi_pipe_hdl;
    ot_eis_handle vi_chn_hdl;
    ot_s32 frame_cnt;
    dis_dump_mode dump_mode;

    ot_vproc_grp vproc_grp;
    ot_vproc_chn vproc_chn;
    ot_eis_handle vproc_grp_hdl;
    ot_eis_handle vproc_chn_hdl;

    ot_u32 orig_depth;
    ot_eis_vproc_work_mode orig_chn_mode;

    ot_eis_video_frame frame;
    ot_char *user_page_addr[2]; /* 2 Y and C */
    ot_u32 size;
    ot_u32 c_size;
    ot_u32 depth_flag;

    ot_u64 begin_pts;
    ot_u64 end_pts;
    ot_msensor_buf_attr msensor_buf_attr;

    FILE *pts_fd;
    FILE *gyro_fd;
    FILE *acc_fd;
    FILE *hv_fd;
    FILE *yuv_fd;
    FILE *stream_fd;

    ot_s32 picture_cnt;

    list_exp *head;
} dis_dump_info;

static sample_venc_attr g_sample_venc_attr = {
    .venc_attr = {
        .codecFormat = OT_EIS_VENC_CODEC_HEVC,
        .common_attr = {
            .max_pic_width = PIC_WIDTH,
            .max_pic_height = PIC_HEIGHT,
            .out_width = PIC_WIDTH,
            .out_height = PIC_HEIGHT,
            .stream_buf_size = OT_ALIGN_UP(VENC_BUF_SIZE, VENC_STRIDE),
        },
        .video_attr = {
            .rc_attr = {
                .gop = VENC_FRAME_RATE,
                .output_fps = VENC_FRAME_RATE,
                .input_fps = VENC_FRAME_RATE,
                .rc_mode = OT_EIS_VENC_RC_MODE_CBR,
                .cbr_attr = {
                    .statis_time = 1,
                    .target_bit_rate = 10563,
                },
            },
            .gop_attr = {
                .gop_mode = OT_EIS_VENC_GOP_MODE_NORMAL,
                .gop_normal = {
                    .qp_delta_ip = DUMP_FRAME_DEPTH,
                },
            },
        },
    },
    .venc_hdl = (ot_eis_venc_chn_handle)-1,
};

static ot_void list_add_node(ot_u64 pts, dis_dump_isp_info *dis_isp, list_exp *head)
{
    list_exp *new_node = OT_NULL;
    new_node = (list_exp *)malloc(sizeof(list_exp));
    if (new_node == OT_NULL) {
        printf("malloc exp node failed!\n");
        return;
    }
    new_node->pts = pts;
    new_node->exposuretime = dis_isp->exposuretime;
    new_node->h_max = dis_isp->hmax;
    new_node->v_max = dis_isp->vmax;

    new_node->next = head->next;
    head->next = new_node;
    g_exposuretime_tmp = dis_isp->exposuretime;
    g_h_max_tmp = dis_isp->hmax;
    g_v_max_tmp = dis_isp->vmax;
}

static list_exp* list_find_node(ot_u64 pts, dis_dump_isp_info *dis_isp, list_exp *head)
{
    list_exp *tmp_node = {0};
    tmp_node = head->next;

    while (tmp_node != OT_NULL) {
        if (tmp_node->pts == pts) {
            dis_isp->exposuretime = tmp_node->exposuretime;
            dis_isp->hmax = tmp_node->h_max;
            dis_isp->vmax = tmp_node->v_max;
            return tmp_node;
        }
        tmp_node = tmp_node->next;
    }
    //printf("not find exposuretime, pts:%llu, use previous frame exposuretime\n", pts);
    dis_isp->exposuretime = g_exposuretime_tmp;
    dis_isp->hmax = g_h_max_tmp;
    dis_isp->vmax = g_v_max_tmp;
    return OT_NULL;
}

static ot_void list_del_node(list_exp *old_node, list_exp *head)
{
    list_exp *tmp_node = {0};
    tmp_node = head;

    while (tmp_node != OT_NULL) {
        if (tmp_node->next == old_node) {
            tmp_node->next = old_node->next;
            free(old_node);
            old_node = OT_NULL;
            return;
        }
        tmp_node = tmp_node->next;
    }
}

static ot_char *dis_dump_get_file_suffix_name(ot_eis_img_pixel_format pixel_format)
{
    switch (pixel_format) {
        case OT_EIS_IMAGE_FORMAT_YVU_422_SEMIPLANAR:
        case OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR:
        case OT_EIS_IMAGE_FORMAT_YUV_400:
            return "yuv";
        case OT_EIS_IMAGE_FORMAT_BAYER_8BPP:
        case OT_EIS_IMAGE_FORMAT_BAYER_10BPP:
        case OT_EIS_IMAGE_FORMAT_BAYER_12BPP:
        case OT_EIS_IMAGE_FORMAT_BAYER_14BPP:
        case OT_EIS_IMAGE_FORMAT_BAYER_16BPP:
            return "raw";
        default:
            return "na";
    }
}

static ot_char *dis_dump_get_pixel_format_str(ot_eis_img_pixel_format pixel_format)
{
    switch (pixel_format) {
        case OT_EIS_IMAGE_FORMAT_YVU_422_SEMIPLANAR:
            return "P422";
        case OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR:
            return "P420";
        case OT_EIS_IMAGE_FORMAT_YUV_400:
            return "P400";
        case OT_EIS_IMAGE_FORMAT_BAYER_8BPP:
            return "Raw8";
        case OT_EIS_IMAGE_FORMAT_BAYER_10BPP:
            return "Raw10";
        case OT_EIS_IMAGE_FORMAT_BAYER_12BPP:
            return "Raw12";
        case OT_EIS_IMAGE_FORMAT_BAYER_14BPP:
            return "Raw14";
        case OT_EIS_IMAGE_FORMAT_BAYER_16BPP:
            return "Raw16";
        default:
            return "na";
    }
}

static ot_s32 dis_dump_make_frame_file_name(dis_dump_info *dump_info)
{
    ot_char yuv_name[FILE_NAME_LENGTH];
    ot_char vi_name[FILE_NAME_SHORT] = "VI";
    ot_char vproc_name[FILE_NAME_SHORT] = "VPROC";
    ot_s32 pipe = dump_info->dis_enable ? dump_info->vproc_grp : dump_info->vi_pipe;
    ot_s32 chn = dump_info->dis_enable ? dump_info->vproc_chn : dump_info->chn;

    /* make file name */
    if (snprintf_s(yuv_name, FILE_NAME_LENGTH, FILE_NAME_LENGTH - 1, "./out%s_pipe%d_chn%d_%ux%u_%s_%d.%s",
        dump_info->dis_enable ? vproc_name : vi_name,
        pipe, chn, dump_info->frame.attr.width, dump_info->frame.attr.height,
        dis_dump_get_pixel_format_str(dump_info->frame.attr.pixel_fmt),
        dump_info->frame_cnt,
        dis_dump_get_file_suffix_name(dump_info->frame.attr.pixel_fmt)) == -1) {
        printf("set output file name failed!\n");
        return OT_FAILURE;
    }

    printf("Dump YUV frame of %s pipe %d chn %d to file: \"%s\"\n", dump_info->dis_enable ? vproc_name : vi_name,
        pipe, chn, yuv_name);

    /* open file */
    dump_info->yuv_fd = fopen(yuv_name, "wb");
    if (dump_info->yuv_fd == OT_NULL) {
        printf("open file failed:%s!\n", strerror(errno));
        return OT_FAILURE;
    }
    (ot_void)fflush(stdout);
    return OT_SUCCESS;
}

static ot_s32 dis_dump_set_vproc_depth(dis_dump_info *dump_info)
{
    const ot_u32 depth = 2;
    ot_s32 ret;
    ot_eis_vproc_chn_attr chn_attr;
    ot_vproc_chn chn = dump_info->vproc_chn;  //vproc_chn_id
    ot_eis_handle vproc_chn_hdl = dump_info->vproc_chn_hdl;

    ret = ot_eis_vproc_chn_get_attr(vproc_chn_hdl, &chn_attr);
    if (ret != OT_SUCCESS) {
        printf("get chn attr error!!!\n");
        return OT_FAILURE;
    }

    dump_info->orig_depth = chn_attr.frame_queue_depth;
    dump_info->orig_chn_mode = chn_attr.mode;
    chn_attr.frame_queue_depth = depth;
    chn_attr.mode = OT_EIS_VPROC_WORK_MODE_USER;

    if (ot_eis_vproc_chn_set_attr(vproc_chn_hdl, &chn_attr) != OT_SUCCESS) {
        printf("set chn attr error!!!\n");
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}

static ot_s32 dis_dump_set_chn_depth(dis_dump_info *dump_info)
{
    if (dump_info->dis_enable == OT_FALSE) {
        if (dump_info->chn < OT_VI_MAX_PHYS_CHN_NUM) {
            ot_eis_vi_chn_attr chn_attr = { 0 };
            if (ot_eis_vi_get_chn_attr(dump_info->vi_chn_hdl, &chn_attr) != OT_SUCCESS) {
                return OT_FAILURE;
            }

            dump_info->orig_depth = chn_attr.frame_queue_depth;
            chn_attr.frame_queue_depth = DUMP_FRAME_DEPTH;

            if (ot_eis_vi_chn_enable_dump(dump_info->vi_chn_hdl, OT_TRUE, chn_attr.frame_queue_depth) != OT_SUCCESS) {
                return OT_FAILURE;
            }
        } else {
            printf("vi_chn %d err\n", dump_info->chn);
            return OT_FAILURE;
        }
    } else {
        if (dis_dump_set_vproc_depth(dump_info) != OT_SUCCESS) {
            return OT_FAILURE;
        }
    }

    return OT_SUCCESS;
}

static ot_s32 dis_dump_get_chn_frame(dis_dump_info *dump_info, ot_s32 milli_sec)
{
    ot_s32 ret;

    if (dump_info->dis_enable == OT_TRUE) {
        ret = ot_eis_vproc_chn_acquire_frame(dump_info->vproc_chn_hdl, &dump_info->frame, milli_sec);
        printf("vproc_get_frame\n");
        if (ret != OT_SUCCESS) {
            printf("ot_omi_vproc_get_chn_frame failed\n");
            return OT_FAILURE;
        }
    } else {
        ret = ot_eis_vi_chn_get_frame(dump_info->vi_chn_hdl, &dump_info->frame, milli_sec);
        if (ret != OT_SUCCESS) {
            printf("ot_omi_vi_get_chn_frame failed\n");
            return OT_FAILURE;
        }
    }

    return OT_SUCCESS;
}

static ot_s32 dis_dump_release_chn_frame(dis_dump_info *dump_info)
{
    ot_s32 ret;

    if (dump_info->dis_enable == OT_TRUE) {
        ret = ot_eis_vproc_chn_release_frame(dump_info->vproc_chn_hdl, &dump_info->frame);
        if (ret != OT_SUCCESS) {
            printf("ot_eis_vproc_chn_release_frame failed\n");
            return OT_FAILURE;
        }
    } else {
        ret = ot_eis_vi_chn_release_frame(dump_info->vi_chn_hdl, &dump_info->frame);
        if (ret != OT_SUCCESS) {
            printf("ot_eis_vi_chn_release_frame failed\n");
            return OT_FAILURE;
        }
    }

    return OT_SUCCESS;
}


static ot_s32 dis_dump_chn_try_get_frame(dis_dump_info *dump_info)
{
    ot_s32 milli_sec = 300;
    ot_s32 try_times = 10;
    ot_s32 ret;

    while (dis_dump_get_chn_frame(dump_info, milli_sec) != OT_SUCCESS) {
        if (g_signal_flag == 1) {
            printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
            return OT_FAILURE;
        }
        try_times--;
        if (try_times <= 0) {
            printf("get frame error for 10 times,now exit !!!\n");
            return OT_FAILURE;
        }
        usleep(40000); /* 40000 sleep */
    }

    if (dis_dump_make_frame_file_name(dump_info) != OT_SUCCESS) {
        dis_dump_release_chn_frame(dump_info);
        dump_info->frame.pool_hdl = OT_NULL;
        return OT_FAILURE;
    }

    ret = dis_dump_release_chn_frame(dump_info);
    if (ret != OT_SUCCESS) {
        printf("Release frame error ,now exit !!!\n");
        return OT_FAILURE;
    }

    dump_info->frame.pool_hdl = OT_NULL;

    return OT_SUCCESS;
}

static ot_void dis_dump_covert_chroma_sp42x_to_planar(const ot_eis_img_frame *frame, FILE *fd,
                                                      ot_u32 uv_height, ot_bool is_uv_invert, dis_dump_info *dump_info)
{
    /* If this value is too small and the image is big, this memory may not be enough */
    unsigned char tmp_buf[MAX_FRM_WIDTH];
    char *mem_content = OT_NULL;
    char *virt_addr_c = OT_NULL;
    ot_u32 w, h;
    ot_phys_addr_t phys_addr;

    phys_addr = frame->buff.phys_addr[1];
    if (ot_smr_mmap(phys_addr, dump_info->c_size, OT_FALSE, (ot_void **)&dump_info->user_page_addr[1]) != 0) {
        printf("mmap chroma data error!!!\n");
        return;
    }
    virt_addr_c = dump_info->user_page_addr[1];

    (ot_void)fflush(fd);
    /* save U */
    (ot_void)fprintf(stderr, "U......");
    (ot_void)fflush(stderr);
    for (h = 0; h < uv_height; h++) {
        mem_content = virt_addr_c + h * frame->buff.stride[1];
        if (!is_uv_invert) {
            mem_content += 1;
        }

        for (w = 0; w < frame->attr.width / 2; w++) { /* 2 chroma width */
            tmp_buf[w] = *mem_content;
            mem_content += 2; /* 2 semiplanar steps */
        }

        (ot_void)fwrite(tmp_buf, frame->attr.width / 2, 1, fd); /* 2 chroma width */
    }
    (ot_void)fflush(fd);

    /* save V */
    (ot_void)fprintf(stderr, "V......");
    (ot_void)fflush(stderr);
    for (h = 0; h < uv_height; h++) {
        mem_content = virt_addr_c + h * frame->buff.stride[1];
        if (is_uv_invert) {
            mem_content += 1;
        }

        for (w = 0; w < frame->attr.width / 2; w++) { /* 2 chroma width */
            tmp_buf[w] = *mem_content;
            mem_content += 2; /* 2 semiplanar steps */
        }

        (ot_void)fwrite(tmp_buf, frame->attr.width / 2, 1, fd); /* 2 chroma width */
    }

    (ot_void)fflush(fd);
    if (dump_info->user_page_addr[1] != OT_NULL) {
        ot_smr_munmap(dump_info->user_page_addr[1], dump_info->c_size);
        dump_info->user_page_addr[1] = OT_NULL;
    }
}

/* When saving a file, sp420 will be denoted by p420 and sp422 will be denoted by p422 in the name of the file */
static ot_void dis_dump_yuv_8bit_dump(ot_eis_img_frame *frame, FILE *fd, dis_dump_info *dump_info)
{
    unsigned int h;
    char *virt_addr_y = OT_NULL;
    char *mem_content = OT_NULL;
    ot_phys_addr_t phys_addr;
    ot_eis_img_pixel_format pixel_format = frame->attr.pixel_fmt;
    /* When the storage format is a planar format, this variable is used to keep the height of the UV component */
    ot_u32 uv_height = 0;
    ot_bool is_uv_invert = (pixel_format == OT_EIS_IMAGE_FORMAT_YUV_420_SEMIPLANAR ||
        pixel_format == OT_EIS_IMAGE_FORMAT_YUV_422_SEMIPLANAR) ? OT_TRUE : OT_FALSE;

    dump_info->size = (frame->buff.stride[0]) * (frame->attr.height);
    if (pixel_format == OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR || pixel_format == OT_EIS_IMAGE_FORMAT_YUV_420_SEMIPLANAR) {
        dump_info->c_size = (frame->buff.stride[1]) * (frame->attr.height) / 2; /* 2 uv height */
        uv_height = frame->attr.height / 2; /* 2 uv height */
    } else if (pixel_format == OT_EIS_IMAGE_FORMAT_YVU_422_SEMIPLANAR ||
        pixel_format == OT_EIS_IMAGE_FORMAT_YUV_422_SEMIPLANAR) {
        dump_info->c_size = (frame->buff.stride[1]) * (frame->attr.height);
        uv_height = frame->attr.height;
    } else if (pixel_format == OT_EIS_IMAGE_FORMAT_YUV_400) {
        dump_info->c_size = 0;
        uv_height = frame->attr.height;
    }

    phys_addr = frame->buff.phys_addr[0];
    if (ot_smr_mmap(phys_addr, dump_info->size, OT_FALSE, (ot_void **)&dump_info->user_page_addr[0]) != 0) {
        printf("mmap luma data error!!!\n");
        return;
    }
    virt_addr_y = dump_info->user_page_addr[0];

    /* save Y */
    (ot_void)fprintf(stderr, "saving......Y......");
    (ot_void)fflush(stderr);

    for (h = 0; h < frame->attr.height; h++) {
        mem_content = virt_addr_y + h * frame->buff.stride[0];
        (ot_void)fwrite(mem_content, frame->attr.width, 1, fd);
    }

    (ot_void)fflush(fd);
    if (pixel_format != OT_EIS_IMAGE_FORMAT_YUV_400) {
        dis_dump_covert_chroma_sp42x_to_planar(frame, fd, uv_height, is_uv_invert, dump_info);
    }

    (ot_void)fflush(stderr);
    ot_smr_munmap(dump_info->user_page_addr[0], dump_info->size);
    dump_info->user_page_addr[0] = OT_NULL;
}

static ot_s32 dis_dump_start_msensormng(ot_msensor_param *msensor_param)
{
    ot_s32 ret;
    ot_s32 msensor_chipfd = -1;

    msensor_chipfd = open("/dev/motionsensor_chip", O_RDWR);
    if (msensor_chipfd < 0) {
        printf("open motionsensor_chip failed\n");
        return OT_FAILURE;
    }

    g_msensor_mngfd = open("/dev/motionsensor_mng", O_RDWR);
    if (g_msensor_mngfd < 0) {
        printf("open motionsensor_mng failed\n");
        goto exit1;
    }

    ret = ioctl(msensor_chipfd, MSENSOR_CMD_GET_PARAM, msensor_param);  //失败
    if (ret != OT_SUCCESS) {
        printf("msensor get param failed!\n");
        goto exit2;
    }
    close(msensor_chipfd);

    ret = ioctl(g_msensor_mngfd, MSENSOR_CMD_ADD_USER, &g_msensor_userfd);
    if (ret != OT_SUCCESS) {
        printf("MSENSOR_CMD_ADD_USER failed, ret:%x !\n", ret);
        goto exit1;
    }

    return  OT_SUCCESS;
exit2:
    close(g_msensor_mngfd);
    g_msensor_mngfd = -1;
exit1:
    close(msensor_chipfd);
    return OT_FAILURE;
}


static ot_void dis_dump_stop_msensormng(dis_dump_info *dump_info)
{
    if (g_msensor_userfd > 0) {
        ioctl(g_msensor_mngfd, MSENSOR_CMD_DELETE_USER, &g_msensor_userfd);
        g_msensor_userfd = -1;
    }

    if (g_msensor_mngfd > 0) {
        close(g_msensor_mngfd);
        g_msensor_mngfd = -1;
    }

    if (msensor_param_fd != OT_NULL) {
        fclose(msensor_param_fd);
        msensor_param_fd = OT_NULL;
    }

    if (ldc_fd != OT_NULL) {
        fclose(ldc_fd);
        ldc_fd = OT_NULL;
    }

    if (dump_info->gyro_fd != OT_NULL) {
        fclose(dump_info->gyro_fd);
        dump_info->gyro_fd = OT_NULL;
    }

    if (dump_info->pts_fd != OT_NULL) {
        fclose(dump_info->pts_fd);
        dump_info->pts_fd = OT_NULL;
    }

    if (dump_info->acc_fd != OT_NULL) {
        fclose(dump_info->acc_fd);
        dump_info->acc_fd = OT_NULL;
    }

    if (dump_info->hv_fd != OT_NULL) {
        fclose(dump_info->hv_fd);
        dump_info->hv_fd = OT_NULL;
    }

    if (dump_info->msensor_buf_attr.virt_addr != OT_NULL) {
        ot_smr_munmap((ot_void *)(ot_uintptr_t)dump_info->msensor_buf_attr.virt_addr,
            dump_info->msensor_buf_attr.buf_len);
    }

    printf("dis_dump_stop_msensormng success\n");
}

static ot_s32 dis_dump_mfusion_get_gyro_config(const ot_u32 fusion_id,
                                               ot_eis_mfusion_six_side_calibration *six_side_calibration,
                                               ot_eis_mfusion_drift *drift,
                                               ot_eis_mfusion_temperature_drift *temperature_drift)
{
    if (ot_eis_mfusion_get_gyro_six_side_calibration(fusion_id, six_side_calibration) != OT_SUCCESS) {
        printf("dis_dump_get_gyro_six_side_calibration failed\n");
        return OT_FAILURE;
    }

    if (ot_eis_mfusion_get_gyro_online_drift(fusion_id, drift) != OT_SUCCESS) {
        printf("dis_dump_get_gyro_six_side_calibration failed\n");
        return OT_FAILURE;
    }

    if (ot_eis_mfusion_get_gyro_online_temperature_drift(fusion_id, temperature_drift) != OT_SUCCESS) {
        printf("dis_dump_get_gyro_six_side_calibration failed\n");
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}


static ot_s32 dis_dump_get_msensordata(ot_msensor_data_type data_type, ot_u64 begin_pts, ot_u64 end_pts,
                                       dis_dump_info *dump_info)
{
    ot_s32 i, ret = OT_SUCCESS;
    ot_u32 j, num;
    ot_char data_buf[FILE_NAME_LENGTH] = {0};
    ot_s32 *x_viraddr = OT_NULL;
    ot_s32 *y_viraddr = OT_NULL;
    ot_s32 *z_viraddr = OT_NULL;
    ot_s32 *temp_viraddr = OT_NULL;
    ot_u64 *pts_viraddr = OT_NULL;
    ot_msensor_data_info msensor_data_info = {0};
    msensor_data_info.id = g_msensor_userfd;
    msensor_data_info.data_type = data_type;
    msensor_data_info.begin_pts = begin_pts;
    msensor_data_info.end_pts = end_pts;

    ret = ioctl(g_msensor_mngfd, MSENSOR_CMD_GET_DATA, &msensor_data_info);
    if (ret != OT_SUCCESS) {
        printf("msensor_cmd_get_data failed\n");
        return OT_FAILURE;
    }

    for (i = 0; i < 2; i++) { /* cyclic buffer 2 */
        num = msensor_data_info.data[i].num;
        x_viraddr = (ot_s32 *)(ot_uintptr_t)(dump_info->msensor_buf_attr.virt_addr +
            ((ot_u64)(ot_uintptr_t)msensor_data_info.data[i].x_phys_addr - dump_info->msensor_buf_attr.phys_addr));
        y_viraddr = (ot_s32 *)(ot_uintptr_t)(dump_info->msensor_buf_attr.virt_addr +
            ((ot_u64)(ot_uintptr_t)msensor_data_info.data[i].y_phys_addr - dump_info->msensor_buf_attr.phys_addr));
        z_viraddr = (ot_s32 *)(ot_uintptr_t)(dump_info->msensor_buf_attr.virt_addr +
            ((ot_u64)(ot_uintptr_t)msensor_data_info.data[i].z_phys_addr - dump_info->msensor_buf_attr.phys_addr));
        temp_viraddr = (ot_s32 *)(ot_uintptr_t)(dump_info->msensor_buf_attr.virt_addr +
            ((ot_u64)(ot_uintptr_t)msensor_data_info.data[i].temperature_phys_addr -
            dump_info->msensor_buf_attr.phys_addr));
        pts_viraddr = (ot_u64 *)(ot_uintptr_t)(dump_info->msensor_buf_attr.virt_addr +
            ((ot_u64)(ot_uintptr_t)msensor_data_info.data[i].pts_phys_addr - dump_info->msensor_buf_attr.phys_addr));
        for (j = 0; j < num; j++) {
            (ot_void)snprintf_s(data_buf, sizeof(data_buf), sizeof(data_buf) - 1, "%d,%d,%d,%d,%llu\n",
                x_viraddr[j], y_viraddr[j], z_viraddr[j], temp_viraddr[j], pts_viraddr[j]);
            (ot_void)fwrite(data_buf, strlen(data_buf), 1,
                (MSENSOR_DATA_GYRO == data_type) ? dump_info->gyro_fd : dump_info->acc_fd);
        }
    }

    ret = ioctl(g_msensor_mngfd, MSENSOR_CMD_RELEASE_BUF, &msensor_data_info);
    if (ret != OT_SUCCESS) {
        printf("MSENSOR_CMD_RELEASE_BUF failed, ret:%x \n", ret);
        return ret;
    }
    return ret;
}

static ot_void dis_dump_write_proc_title(const char *name)
{
    ot_char title[FILE_NAME_LENGTH] = {0};
    ot_s32 ret;

    ret = snprintf_s(title, sizeof(title),
        sizeof(title) - 1, "[%s]\n", name);
    if (ret <= 0) {
        printf("dis_dump_write_proc_title of %s faliure\n", name);
    }
    (ot_void)fwrite(title, strlen(title), 1, msensor_param_fd);
}
static ot_s32 dis_dump_get_mensor_param_config_msensor_param(ot_msensor_param *msensor_param)
{
    ot_s32 ret;
    ot_char msensor_param_attr[FILE_NAME_LENGTH] = {0};
    ot_char msensor_param_config_gyro_config[FILE_NAME_LENGTH] = {0};
    ot_char msensor_param_config_acc_config[FILE_NAME_LENGTH] = {0};
    ret = snprintf_s(msensor_param_attr, sizeof(msensor_param_attr), sizeof(msensor_param_attr) - 1,
        "device_mask=%u\ntemperature_mask=%u\n\n", msensor_param->attr.device_mask,
        msensor_param->attr.temperature_mask);
    if (ret <= 0) {
        printf("snprintf_s msensor_param_attr faliure\n");
        return OT_FAILURE;
    }
    ret = snprintf_s(msensor_param_config_gyro_config, sizeof(msensor_param_config_gyro_config),
        sizeof(msensor_param_config_gyro_config) - 1,
        "odr=%u\nfsr=%u\ndata_width=%u\ntemperature_max=%d\ntemperature_min=%d\n\n",
        msensor_param->config.gyro_config.odr, msensor_param->config.gyro_config.fsr,
        msensor_param->config.gyro_config.data_width, msensor_param->config.gyro_config.temperature_max,
        msensor_param->config.gyro_config.temperature_min);
    if (ret <= 0) {
        printf("snprintf_s msensor_param_config_gyro_config faliure\n");
        return OT_FAILURE;
    }
    ret = snprintf_s(msensor_param_config_acc_config, sizeof(msensor_param_config_acc_config),
        sizeof(msensor_param_config_acc_config) - 1,
        "odr=%u\nfsr=%u\ndata_width=%u\ntemperature_max=%d\ntemperature_min=%d\n\n",
        msensor_param->config.acc_config.odr, msensor_param->config.acc_config.fsr,
        msensor_param->config.acc_config.data_width, msensor_param->config.acc_config.temperature_max,
        msensor_param->config.acc_config.temperature_min);
    if (ret <= 0) {
        printf("snprintf_s msensor_param_config_acc_config faliure\n");
        return OT_FAILURE;
    }
    dis_dump_write_proc_title("msensor_attr");
    (ot_void)fwrite(msensor_param_attr, strlen(msensor_param_attr), 1, msensor_param_fd);
    dis_dump_write_proc_title("gyro_config");
    (ot_void)fwrite(msensor_param_config_gyro_config, strlen(msensor_param_config_gyro_config), 1, msensor_param_fd);
    dis_dump_write_proc_title("acc_config");
    (ot_void)fwrite(msensor_param_config_acc_config, strlen(msensor_param_config_acc_config), 1, msensor_param_fd);
    return OT_SUCCESS;
}

static ot_s32 dis_dump_get_mensor_param_config_calibration(ot_eis_mfusion_six_side_calibration *six_side_calibration,
    ot_eis_mfusion_drift *drift)
{
    ot_s32 ret;
    ot_char six_calibration[FILE_NAME_LENGTH] = {0};
    ot_char gyro_drift_attr[FILE_NAME_LENGTH] = {0};

    ret = snprintf_s(six_calibration, sizeof(six_calibration), sizeof(six_calibration) - 1,
        "enable=%d\nmatrix=\"%d,%d,%d,%d,%d,%d,%d,%d,%d\"\n\n",
        six_side_calibration->enable,
        six_side_calibration->matrix[0], six_side_calibration->matrix[1],    /* index:0,1      */
        six_side_calibration->matrix[2], six_side_calibration->matrix[3],    /* index:2,3      */
        six_side_calibration->matrix[4], six_side_calibration->matrix[5],    /* index:4,5      */
        six_side_calibration->matrix[6], six_side_calibration->matrix[7],    /* index:6,7      */
        six_side_calibration->matrix[8]);    /* index:8      */
    if (ret <= 0) {
        printf("snprintf_s six_calibration faliure\n");
        return OT_FAILURE;
    }

    ret = snprintf_s(gyro_drift_attr, sizeof(gyro_drift_attr), sizeof(gyro_drift_attr) - 1,
        "enable=%d\nmatrix=\"%d,%d,%d\"\n\n",
        drift->enable, drift->drift[0], drift->drift[1], drift->drift[2]); /* index:0,1,2 */
    if (ret <= 0) {
        printf("snprintf_s gyro_drift_attr faliure\n");
        return OT_FAILURE;
    }

    dis_dump_write_proc_title("six_side_calibration");
    (ot_void)fwrite(six_calibration, strlen(six_calibration), 1, msensor_param_fd);
    dis_dump_write_proc_title("gyro_drift_attr");
    (ot_void)fwrite(gyro_drift_attr, strlen(gyro_drift_attr), 1, msensor_param_fd);
    return OT_SUCCESS;
}

static ot_s32 dis_dump_get_mensor_param_config_temperature_drift(
    ot_eis_mfusion_temperature_drift *temperature_drift)
{
    ot_s32 i;
    ot_s32 ret;
    ot_char temperature_drift_attr[FILE_NAME_LENGTH] = {0};
    ret = snprintf_s(temperature_drift_attr, sizeof(temperature_drift_attr), sizeof(temperature_drift_attr) - 1,
        "enable=%d\nmode=%d\n;DRIFT_CURV = 0\n;DRIFT_LUT = 1\nrange_min=%d\nrange_max=%d\nstep=%u\n",
        temperature_drift->enable, temperature_drift->mode, temperature_drift->temperature_lut.range_min,
        temperature_drift->temperature_lut.range_max,
        temperature_drift->temperature_lut.step);
    if (ret <= 0) {
        printf("snprintf_s temperature_drift_attr faliure\n");
        return OT_FAILURE;
    }
    dis_dump_write_proc_title("temperature_drift_attr");
    (ot_void)fwrite(temperature_drift_attr, strlen(temperature_drift_attr), 1, msensor_param_fd);

    ret = snprintf_s(temperature_drift_attr, sizeof(temperature_drift_attr), sizeof(temperature_drift_attr) - 1,
        ";%10s %15s %15s %15s %15s %10s\n", "x", "y", "z", "time", "nearest_temp", "temp");
    if (ret <= 0) {
        printf("snprintf_s temperature_drift_attr_x_y_z faliure\n");
        return OT_FAILURE;
    }
    (ot_void)fwrite(temperature_drift_attr, strlen(temperature_drift_attr), 1, msensor_param_fd);
    ret = snprintf_s(temperature_drift_attr, sizeof(temperature_drift_attr), sizeof(temperature_drift_attr) - 1,
        ";%10s %15s %15s %15s %15s %10s\n", "x", "y", "z", "time", "nearest_temp", "temp");
    ret += snprintf_s(temperature_drift_attr, sizeof(temperature_drift_attr), sizeof(temperature_drift_attr) - 1,
        "temp_lut :\n");
    if (ret <= 0) {
        printf("snprintf_s temperature_drift_attr faliure\n");
        return OT_FAILURE;
    }
    (ot_void)fwrite(temperature_drift_attr, strlen(temperature_drift_attr), 1, msensor_param_fd);
    for (i = 0; i < IMU_LUT_LEN; i++) {
        ret = snprintf_s(temperature_drift_attr, sizeof(temperature_drift_attr), sizeof(temperature_drift_attr) - 1,
        "%10d,%10d,%10d,%15d,%15d,%10u,\\\n", temperature_drift->temperature_lut.imu_lut[i][0], /* index:0 */
        temperature_drift->temperature_lut.imu_lut[i][1], /* index:1 */
        temperature_drift->temperature_lut.imu_lut[i][2], /* index:2 */
        temperature_drift->temperature_lut.gyro_lut_status[i][0], /* index:0 */
        temperature_drift->temperature_lut.imu_lut[i][1], /* index:1 */
        (temperature_drift->temperature_lut.range_min +
        temperature_drift->temperature_lut.step * i) / 1024); /* 1024 step per centigrade */
        if (ret <= 0) {
        printf("snprintf_s temperature_drift_attr_lut faliure\n");
        return OT_FAILURE;
        }
        (ot_void)fwrite(temperature_drift_attr, strlen(temperature_drift_attr), 1, msensor_param_fd);
    }
    return OT_SUCCESS;
}

static ot_s32 dis_dump_get_mensor_param_config(ot_msensor_param *msensor_param,
                                               ot_eis_mfusion_six_side_calibration *six_side_calibration,
                                               ot_eis_mfusion_drift *drift,
                                               ot_eis_mfusion_temperature_drift *temperature_drift)
{
    ot_s32 ret;
    ot_char dis_other_attr[FILE_NAME_LENGTH] = {0};
    ot_char dis_cfg_attr[FILE_NAME_LENGTH] = {0};

    if (dis_dump_get_mensor_param_config_msensor_param(msensor_param) != OT_SUCCESS) {
        printf("dis_dump_get_mensor_param_config1 faliure\n");
        return OT_FAILURE;
    }

    if (dis_dump_get_mensor_param_config_calibration(six_side_calibration, drift) != OT_SUCCESS) {
        printf("dis_dump_get_mensor_param_config2 faliure\n");
        return OT_FAILURE;
    }

    if (dis_dump_get_mensor_param_config_temperature_drift(temperature_drift) != OT_SUCCESS) {
        printf("dis_dump_get_mensor_param_config2 faliure\n");
        return OT_FAILURE;
    }

    ret = snprintf_s(dis_other_attr, sizeof(dis_other_attr), sizeof(dis_other_attr) - 1,
        "enable=%d\ntimelag=%d\nstrength=%u\n\n",
        dis_attr.enable, dis_attr.timelag, dis_attr.strength);
    if (ret <= 0) {
        printf("snprintf_s dis_other_attr faliure\n");
        return OT_FAILURE;
    }

    ret = snprintf_s(dis_cfg_attr, sizeof(dis_cfg_attr), sizeof(dis_cfg_attr) - 1,
        "pdt_type=%d\n;IPC = 0\n;DV = 1\ncrop_ratio=%u\nframe_rate=%u\ncamera_steady=%d\nscale=%d\n",
        dis_cfg.pdt_type, dis_cfg.crop_ratio, dis_cfg.frame_rate, dis_cfg.camera_steady, dis_cfg.scale);
    if (ret <= 0) {
        printf("snprintf_s dis_cfg_attr faliure\n");
        return OT_FAILURE;
    }

    dis_dump_write_proc_title("dis_attr");
    (ot_void)fwrite(dis_other_attr, strlen(dis_other_attr), 1, msensor_param_fd);
    dis_dump_write_proc_title("dis_cfg");
    (ot_void)fwrite(dis_cfg_attr, strlen(dis_cfg_attr), 1, msensor_param_fd);

    printf("fwrite =====mensor_param_config success=====\n");
    return OT_SUCCESS;
}

static ot_s32 dis_dump_get_ldc_param_config(const ot_eis_dis_attr *dis_attr)
{
    ot_char dis_ldc[FILE_NAME_LENGTH * 2] = {0};
    ot_char src_calibration_ratio[FILE_NAME_LENGTH] = {0};
    ot_char dst_calibration_ratio[FILE_NAME_LENGTH] = {0};
    ot_s32 ret;

    ret = snprintf_s(dis_ldc, sizeof(dis_ldc), sizeof(dis_ldc) - 1,
    "[dis_ldc_en]\nenable=%d\n\n[dis_ldc_attr]\nfocal_len_x=%d\nfocal_len_y=%d\n"
    "coord_shift_x=%d\ncoord_shift_y=%d\nmax_du=%d\n",
    dis_attr->dis_ldc_en, dis_attr->dis_ldc_attr.focal_len_x,
    dis_attr->dis_ldc_attr.focal_len_y, dis_attr->dis_ldc_attr.coord_shift_x,
    dis_attr->dis_ldc_attr.coord_shift_y, dis_attr->dis_ldc_attr.max_du);
    if (ret <= 0) {
        printf("snprintf_s dis_ldc faliure\n");
        return OT_FAILURE;
    }

    ret = snprintf_s(src_calibration_ratio, sizeof(src_calibration_ratio), sizeof(src_calibration_ratio) - 1,
    "src_calibration_ratio=\"%d, %d, %d, %d, %d, %d, %d, %d, %d\"\n",
    dis_attr->dis_ldc_attr.src_calibration_ratio[0], dis_attr->dis_ldc_attr.src_calibration_ratio[1], /* index:0,1 */
    dis_attr->dis_ldc_attr.src_calibration_ratio[2], dis_attr->dis_ldc_attr.src_calibration_ratio[3], /* index:2,3 */
    dis_attr->dis_ldc_attr.src_calibration_ratio[4], dis_attr->dis_ldc_attr.src_calibration_ratio[5], /* index:4,5 */
    dis_attr->dis_ldc_attr.src_calibration_ratio[6], dis_attr->dis_ldc_attr.src_calibration_ratio[7], /* index:6,7 */
    dis_attr->dis_ldc_attr.src_calibration_ratio[8]); /* index:8 */
    if (ret <= 0) {
        printf("snprintf_s src_calibration_ratio faliure\n");
        return OT_FAILURE;
    }

    ret = snprintf_s(dst_calibration_ratio, sizeof(dst_calibration_ratio), sizeof(dst_calibration_ratio) - 1,
    "dst_calibration_ratio=\"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\"\n",
    dis_attr->dis_ldc_attr.dst_calibration_ratio[0], dis_attr->dis_ldc_attr.dst_calibration_ratio[1], /* index:0,1 */
    dis_attr->dis_ldc_attr.dst_calibration_ratio[2], dis_attr->dis_ldc_attr.dst_calibration_ratio[3], /* index:2,3 */
    dis_attr->dis_ldc_attr.dst_calibration_ratio[4], dis_attr->dis_ldc_attr.dst_calibration_ratio[5], /* index:4,5 */
    dis_attr->dis_ldc_attr.dst_calibration_ratio[6], dis_attr->dis_ldc_attr.dst_calibration_ratio[7], /* index:6,7 */
    dis_attr->dis_ldc_attr.dst_calibration_ratio[8], dis_attr->dis_ldc_attr.dst_calibration_ratio[9], /* index:8,9 */
    dis_attr->dis_ldc_attr.dst_calibration_ratio[10], /* index:10 */
    dis_attr->dis_ldc_attr.dst_calibration_ratio[11], /* index:11 */
    dis_attr->dis_ldc_attr.dst_calibration_ratio[12], /* index:12 */
    dis_attr->dis_ldc_attr.dst_calibration_ratio[13]); /* index:13 */
    if (ret <= 0) {
        printf("snprintf_s dst_calibration_ratio faliure\n");
        return OT_FAILURE;
    }

    (ot_void)fwrite(dis_ldc, strlen(dis_ldc), 1, ldc_fd);
    (ot_void)fwrite(src_calibration_ratio, strlen(src_calibration_ratio), 1, ldc_fd);
    (ot_void)fwrite(dst_calibration_ratio, strlen(dst_calibration_ratio), 1, ldc_fd);
    printf("fwrite =====get_ldc_param_config success=====\n");

    return OT_SUCCESS;
}

static ot_s32 dis_dump_get_gyro_data(dis_dump_info *dump_info, ot_u64 pts, dis_dump_isp_info *dis_isp)
{
    ot_s32 i;
    ot_char pts_buf[FILE_NAME_LENGTH] = {0};
    ot_char h_v_buf[FILE_NAME_LENGTH] = {0};

    for (i = MSENSOR_DATA_GYRO; i <= MSENSOR_DATA_ACC; i++) {
        dump_info->end_pts = pts;
        if (dump_info->begin_pts == 0) {
            dump_info->begin_pts = pts - 1;
        } else {
            //上一次帧结束的位置，到本次帧的时间戳, 大约33ms
            if (dis_dump_get_msensordata(i, dump_info->begin_pts, dump_info->end_pts, dump_info) != OT_SUCCESS) {
                return OT_FAILURE;
            }
            if (i == MSENSOR_DATA_ACC) {
                dump_info->begin_pts = dump_info->end_pts + 1;
            }
        }
    }

    (ot_void)snprintf_s(pts_buf, sizeof(pts_buf), sizeof(pts_buf) - 1, "%u,%llu\n", dis_isp->exposuretime, pts);
    (ot_void)snprintf_s(h_v_buf, sizeof(h_v_buf), sizeof(h_v_buf) - 1, "%u,%u\n", dis_isp->hmax, dis_isp->vmax);
    (ot_void)fwrite(pts_buf, strlen(pts_buf), 1, dump_info->pts_fd);
    (ot_void)fwrite(h_v_buf, strlen(h_v_buf), 1, dump_info->hv_fd);
    printf("fwrite =====gyro/acc/pts data success=====\n");

    return OT_SUCCESS;
}

static ot_s32 dis_dump_get_isp_exposuretime(dis_dump_isp_info *dis_isp, ot_eis_video_frame *frame_info)
{
    ot_isp_frame_info *isp_frame_info = OT_NULL;
    ot_u64 isp_phyaddr = frame_info->extra_attr.isp_attr_phys_addr;

    if (ot_smr_mmap(isp_phyaddr, sizeof(ot_isp_frame_info), OT_TRUE, (ot_void **)&isp_frame_info) != 0) {
        printf("ot_smr_mmap failed at line %d\n", __LINE__);
        return OT_FAILURE;
    }

    dis_isp->exposuretime = isp_frame_info->exposure_time[0];
    dis_isp->hmax = isp_frame_info->hmax_times;
    dis_isp->vmax = isp_frame_info->vmax;
    ot_smr_munmap(isp_frame_info, sizeof(ot_isp_frame_info));

    return OT_SUCCESS;
}

static ot_s32 dis_dump_chn_get_frame(dis_dump_info *dump_info)
{
    ot_s32 ret;
    ot_u32 cnt = dump_info->frame_cnt;
    ot_s32 milli_sec = -1;
    dis_dump_isp_info dis_isp = { 0 };

    /* get frame */
    while (cnt--) {
        if (g_signal_flag == 1) {
            printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
            return OT_FAILURE;
        }
        if (dis_dump_get_chn_frame(dump_info, milli_sec) != OT_SUCCESS) {
            printf("Get frame fail \n");
            usleep(1000); /* 1000 sleep */
            continue;
        }

        if (dump_info->dis_enable == OT_TRUE) {
            if (dis_dump_get_isp_exposuretime(&dis_isp, &dump_info->frame) != OT_SUCCESS) {
                dis_dump_release_chn_frame(dump_info);
                dump_info->frame.pool_hdl = OT_NULL;
                return OT_FAILURE;
            }

            if (dis_dump_get_gyro_data(dump_info, dump_info->frame.pts, &dis_isp) != OT_SUCCESS) {
                printf("dis_dump_get_gyro_data faliure\n");
                dis_dump_release_chn_frame(dump_info);
                dump_info->frame.pool_hdl = OT_NULL;
                return OT_FAILURE;
            }
        }

        dis_dump_yuv_8bit_dump(&dump_info->frame, dump_info->yuv_fd, dump_info);

        printf("Get frame %u!!\n", cnt);
        /* release frame after using */
        ret = dis_dump_release_chn_frame(dump_info);
        if (ret != OT_SUCCESS) {
            printf("Release frame error ,now exit !!!\n");
            return ret;
        }
        dump_info->frame.pool_hdl = OT_NULL;
    }
    return OT_SUCCESS;
}

static ot_void dis_dump_restore_vproc_default_depth(dis_dump_info *dump_info)
{
    ot_s32 ret;
    ot_eis_vproc_chn_attr chn_attr;

    ret = ot_eis_vproc_chn_get_attr(dump_info->vproc_chn_hdl, &chn_attr);
    if (ret != OT_SUCCESS) {
        printf("get chn attr error!!!\n");
    }

    chn_attr.frame_queue_depth = dump_info->orig_depth;
    chn_attr.mode = dump_info->orig_chn_mode;
    ret = ot_eis_vproc_chn_set_attr(dump_info->vproc_chn_hdl, &chn_attr);
    if (ret != OT_SUCCESS) {
        printf("set chn attr error!!!\n");
    }
}

static ot_s32 dis_dump_restore_chn_default_depth(dis_dump_info *dump_info)
{
    if (dump_info->dis_enable == OT_FALSE) {
        if (dump_info->chn < OT_VI_MAX_PHYS_CHN_NUM) {
            ot_eis_vi_chn_attr chn_attr = { 0 };
            if (ot_eis_vi_get_chn_attr(dump_info->vi_chn_hdl, &chn_attr) != OT_SUCCESS) {
                return OT_FAILURE;
            }

            chn_attr.frame_queue_depth = dump_info->orig_depth;
            if (ot_eis_vi_chn_enable_dump(dump_info->vi_chn_hdl, OT_TRUE, chn_attr.frame_queue_depth) != OT_SUCCESS) {
                return OT_FAILURE;
            }
        } else {
            printf("vi_chn %d err\n", dump_info->chn);
            return OT_FAILURE;
        }
    } else {
        dis_dump_restore_vproc_default_depth(dump_info);
    }

    return OT_SUCCESS;
}

static ot_void dis_dump_restore(dis_dump_info *dump_info)
{
    ot_s32 ret;

    if (dump_info->frame.pool_hdl != OT_NULL) {
        ret = dis_dump_release_chn_frame(dump_info);
        if (ret != OT_SUCCESS) {
            printf("Release Chn Frame error!!!\n");
        }
        dump_info->frame.pool_hdl = OT_NULL;
    }

    if (dump_info->user_page_addr[0] != OT_NULL) {
        ot_smr_munmap(dump_info->user_page_addr[0], dump_info->size);
        dump_info->user_page_addr[0] = OT_NULL;
    }
    if (dump_info->user_page_addr[1] != OT_NULL) {
        ot_smr_munmap(dump_info->user_page_addr[1], dump_info->size);
        dump_info->user_page_addr[1] = OT_NULL;
    }

    if (dump_info->yuv_fd != OT_NULL) {
        (ot_void)fclose(dump_info->yuv_fd);
        dump_info->yuv_fd = OT_NULL;
    }

    if (dump_info->depth_flag) {
        ret = dis_dump_restore_chn_default_depth(dump_info);
        if (ret != OT_SUCCESS) {
            printf("restore chn depth error!!!\n");
        }
        dump_info->depth_flag = 0;
    }
}

void *dis_dump_start_get_yuv(ot_void *p)
{
    ot_s32 ret;
    dis_dump_info *dump_info = OT_NULL;
    dump_info = (dis_dump_info *)p;

    ret = dis_dump_set_chn_depth(dump_info);
    if (ret != OT_SUCCESS) {
        printf("set chn dump depth failed!\n");
        return OT_NULL;
    }

    dump_info->depth_flag = 1;

    if (memset_s(&dump_info->frame, sizeof(ot_eis_video_frame), 0, sizeof(ot_eis_video_frame)) != EOK) {
        printf("memset_s frame error!!!\n");
        goto exit;
    }
    dump_info->frame.pool_hdl = OT_NULL;

    if (dis_dump_chn_try_get_frame(dump_info) != OT_SUCCESS) {
        goto exit;
    }

    if (dis_dump_chn_get_frame(dump_info) != OT_SUCCESS) {
        goto exit;
    }

exit:
    dis_dump_restore(dump_info);
    return OT_NULL;
}

static ot_s32 test_sample_comm_venc_save_stream(FILE* fd, ot_eis_venc_stream* stream)
{
    fwrite(stream->stream_virt_addr, stream->stream_lens, 1, fd);
    fflush(fd);

    return OT_SUCCESS;
}

static ot_s32 test_sample_comm_save_frame_to_file(FILE* fd, ot_eis_venc_stream* stream)
{
    ot_s32 ret;

    ret = test_sample_comm_venc_save_stream(fd, stream);
    if (ret != OT_SUCCESS) {
        printf("save stream failed!\n");
        return ret;
    }

    return OT_SUCCESS;
}

static ot_s32 dis_dump_save_frame_to_file(ot_eis_venc_stream *stream, dis_dump_info *dump_info)
{
    ot_s32 ret;

    ret = test_sample_comm_save_frame_to_file(dump_info->stream_fd, stream);
    if (ret != OT_SUCCESS) {
        printf("save stream failed!\n");
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}

static ot_s32 dis_dump_set_name_save_stream(ot_s32 venc_chn, ot_s32 *venc_fd, dis_dump_info *dump_info)
{
    ot_s32 fd;
    ot_char stream_file[PATH_MAX] = {0};
    ot_char file_name[PATH_MAX];

    if (snprintf_s(file_name, PATH_MAX, PATH_MAX - 1, "./") < 0) {
        return OT_FAILURE;
    }

    if (realpath(file_name, stream_file) == OT_NULL) {
        printf("chn[%d] stream file path error\n", venc_chn);
        return OT_FAILURE;
    }

    if (snprintf_s(stream_file, PATH_MAX, PATH_MAX - 1, "pipe%d_stream_chn%d_date_%s.265", dump_info->vi_pipe, venc_chn, date_str) < 0) {
        return OT_FAILURE;
    }
    if (dump_info->dump_mode == DIS_DUMP_STREAM || dump_info->dump_mode == DIS_DUMP_STREAM_AND_IMU_DATA) {
        dump_info->stream_fd = fopen(stream_file, "wb");
        if (!dump_info->stream_fd) {
            printf("open file[%s] failed!\n", stream_file);
            return OT_FAILURE;
        }
        fd = fileno(dump_info->stream_fd);
        fchmod(fd, S_IRUSR | S_IWUSR);
    }

    return OT_SUCCESS;
}

static ot_void dis_dump_venc_add_pts_node(list_exp *head, ot_eis_handle vi_pipe_hdl)
{
    dis_dump_isp_info dis_isp;
    ot_s32 milli_sec = 3000;
    ot_eis_video_frame frame_info = { 0 };

    if (ot_eis_vi_pipe_enable_dump(vi_pipe_hdl, OT_TRUE, DUMP_FRAME_DEPTH) != OT_SUCCESS) {
        printf("set pipe dump frame attr failed!\n");
        return;
    }

    if (ot_eis_vi_pipe_get_frame(vi_pipe_hdl, &frame_info, 1, milli_sec) != OT_SUCCESS) {
        printf("get pipe frame failed!\n");
        goto exit0;
    }

    if (dis_dump_get_isp_exposuretime(&dis_isp, &frame_info) != OT_SUCCESS) {
        goto exit1;
    }

    list_add_node(frame_info.pts, &dis_isp, head);

exit1:
    if (ot_eis_vi_pipe_release_frame(vi_pipe_hdl, &frame_info, 1) != OT_SUCCESS) {
        printf("ot_eis_vi_pipe_release_frame failed!\n");
    }
exit0:
    if (ot_eis_vi_pipe_enable_dump(vi_pipe_hdl, OT_FALSE, DUMP_FRAME_DEPTH) != OT_SUCCESS) {
        printf("set pipe dump frame attr failed!\n");
        return;
    }
}

static ot_s32 dis_dump_prepare_save_stream(dis_dump_info *dump_info, ot_s32 *venc_fd)
{
    ot_s32 ret;
    ot_s32 node_num;

    if (dis_dump_set_name_save_stream(dump_info->chn, venc_fd, dump_info) != OT_SUCCESS) {
        printf("dis_dump_set_name_save_stream\n");
        return OT_FAILURE;
    }

    dump_info->picture_cnt = 0;

    for (node_num = 0; node_num < TOTAL_NODE_NUM; node_num++) {
        dis_dump_venc_add_pts_node(dump_info->head, dump_info->vi_pipe_hdl);
    }

    ret = ot_eis_venc_chn_stop(g_sample_venc_attr.venc_hdl);
    if (ret != OT_SUCCESS) {
        printf("ot_eis_venc_chn_stop failed\n");
        return OT_FAILURE;
    }

    ret = ot_eis_venc_chn_reset(g_sample_venc_attr.venc_hdl);
    if (ret != OT_SUCCESS) {
        printf("ot_eis_venc_chn_reset failed\n");
        return OT_FAILURE;
    }

    ret = ot_eis_venc_chn_start(g_sample_venc_attr.venc_hdl);
    if (ret != OT_SUCCESS) {
        printf("ot_eis_venc_chn_start failed\n");
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}

typedef struct {
    ot_u32 try_acquire_cnt;
    ot_u32 acquire_succ_cnt;
    ot_u32 try_release_cnt;
    ot_u32 release_succ_cnt;
} media_vproc_stats;
typedef struct {
    ot_eis_handle vproc_chn_hdl;
    ot_eis_handle venc_hdl;
} vproc_send_attr;
static ot_void* sample_vproc_get_frame_send_to_venc_thread(ot_void* param)
{
    ot_s32 ret;
    ot_eis_handle vproc_chn_hdl;
    vproc_send_attr* send_attr = (vproc_send_attr*)param;
    ot_eis_video_frame video_frame = { 0 };
    ot_eis_venc_image_frame venc_frame = { 0 };
    ot_s32 milli_sec = -1;
    vproc_chn_hdl = send_attr->vproc_chn_hdl;
    ot_eis_venc_chn_handle venc_chn_hdl = send_attr->venc_hdl;

    while (g_send_chn_pthread) {
        ret = ot_eis_vproc_chn_acquire_frame(vproc_chn_hdl, &video_frame, milli_sec);
        if (ret != OT_SUCCESS) {
            printf("ot_eis_vproc_chn_acquire_frame failed with%#x\n", ret);
            goto exit;
        }

        venc_frame.frame = &video_frame;
        ret = ot_eis_venc_send_frame(venc_chn_hdl, &venc_frame, 100);
        if (ret != OT_SUCCESS) {
            printf("ot_eis_venc_send_frame failed with %#x\n", ret);
            ot_eis_vproc_chn_release_frame(vproc_chn_hdl, &video_frame);
            goto exit;
        }

        ret = ot_eis_vproc_chn_release_frame(vproc_chn_hdl, &video_frame);
        if (ret != OT_SUCCESS) {
            printf("vproc_chn_release_frame error return %d \n", ret);
            goto exit;
        }
    }

exit:
    return OT_NULL;
}

static ot_s32 sample_comm_get_stream_from_one_channl(sample_venc_attr* attr, dis_dump_info* dump_info)
{
    ot_s32 ret;
    ot_u64 pts;
    ot_eis_venc_chn_status status = { 0 };
    list_exp *head = dump_info->head;
    dis_dump_isp_info dis_isp = { 0 };
    list_exp *old_node = OT_NULL;

    dis_dump_venc_add_pts_node(head, dump_info->vi_pipe_hdl);

    ret = ot_eis_venc_chn_query_status(attr->venc_hdl, &status);
    if (ret != OT_SUCCESS) {
        sample_print("ot_eis_venc_chn_query_status chn[%d] failed with %#x!\n", (ot_s32)((intptr_t)attr->venc_hdl), ret);
        return ret;
    }

    if (status.unTaken_streams == 0) {
        usleep(10000);
        printf("NOTE: current frame is OT_NULL!\n");
        goto fail;
    }

    ot_eis_venc_stream venc_stream = { 0 };
    ret = ot_eis_venc_chn_acquire_stream(attr->venc_hdl, &venc_stream, 3000);
    if (ret != OT_SUCCESS) {
        sample_print("ot_eis_venc_chn_acquire_stream chn[%d] failed with %#x!\n", (ot_s32)((intptr_t)attr->venc_hdl), ret);
        return ret;
    }

    if (dump_info->enable && (dump_info->picture_cnt < dump_info->frame_cnt)) {
        pts = venc_stream.pts;
        old_node = list_find_node(pts, &dis_isp, head);
        if (dump_info->dump_mode == DIS_DUMP_IMU_DATA || dump_info->dump_mode == DIS_DUMP_STREAM_AND_IMU_DATA) {
            if (dis_dump_get_gyro_data(dump_info, pts, &dis_isp) != OT_SUCCESS) {  //get_data
                printf("dis_dump_get_gyro_data faliure\n");
                goto exit2;
            }
        }

        if (old_node != OT_NULL) {
            list_del_node(old_node, head);
        }

        if (dump_info->dump_mode == DIS_DUMP_STREAM || dump_info->dump_mode == DIS_DUMP_STREAM_AND_IMU_DATA) {
            ret = dis_dump_save_frame_to_file(&venc_stream, dump_info);
            if (ret != OT_SUCCESS) {
                goto exit2;
            }
        }

        dump_info->picture_cnt++;
    } else {
        printf("dump finished, print enter key to exit!\n");
    }

    ret = ot_eis_venc_chn_release_stream(attr->venc_hdl, &venc_stream);
    if (ret != OT_SUCCESS) {
        sample_print("ot_eis_venc_chn_release_stream chn[%d] failed with %#x!\n", (ot_s32)((intptr_t)attr->venc_hdl), ret);
        return ret;
    }

    return OT_SUCCESS;

exit2:
    ot_eis_venc_chn_release_stream(attr->venc_hdl, &venc_stream);
fail:
    return ret;
}

/* get stream from each channels and save them */
ot_void* sample_vproc_venc_get_venc_stream_proc(ot_void* p)
{
    ot_s32 ret;
    dis_dump_info *dump_info = OT_NULL;
    dump_info = (dis_dump_info *)p;

    ot_s32 venc_fd;
    if (dis_dump_prepare_save_stream(dump_info, &venc_fd) != OT_SUCCESS) {
        printf("dis_dump_prepare_save_stream failed!\n");
        return OT_NULL;
    }

    while (g_save_file_pthread == OT_TRUE) {
        ret = sample_comm_get_stream_from_one_channl(&g_sample_venc_attr, dump_info);
        if (ret == 1) {
            sample_print("sample_comm_get_stream_from_one_channl chn[%d] JPEG done!\n", (ot_s32)((intptr_t)g_sample_venc_attr.venc_hdl));
            break;
        }

        if (ret != OT_SUCCESS) {
            sample_print("sample_comm_get_stream_from_one_channl chn[%d] failed with %#x!\n", (ot_s32)((intptr_t)g_sample_venc_attr.venc_hdl), ret);
        }
    }

    g_send_chn_pthread = OT_FALSE;
    // fclose(g_file);
    return OT_NULL;
}

static void *dis_dump_start_get_stream(ot_void *p)
{
    ot_s32 ret;
    dis_dump_info *dump_info = OT_NULL;
    dump_info = (dis_dump_info *)p;
    vproc_send_attr send_attr = { 0 };
    pthread_t thread_id = 0;
    pthread_t save_pid = 0;

    ret = sample_comm_venc_start(&g_sample_venc_attr);
    if (ret != OT_SUCCESS) {
        printf("venc start failed with %#x", ret);
        return OT_NULL;
    }
    dump_info->venc_chn_hdl = g_sample_venc_attr.venc_hdl;
    dump_info->chn = (ot_s32)g_sample_venc_attr.venc_hdl;

    send_attr.venc_hdl = g_sample_venc_attr.venc_hdl;
    send_attr.vproc_chn_hdl = dump_info->vproc_chn_hdl;

    /* 初始化链表头 */
    dump_info->head = (list_exp *)malloc(sizeof(list_exp));
    if (dump_info->head == OT_NULL) {
        printf("malloc head failed!\n");
        return OT_NULL;
    }
    memset(dump_info->head, 0, sizeof(list_exp));
    dump_info->head->next = OT_NULL;

    g_send_chn_pthread = OT_TRUE;
    ret = pthread_create(&thread_id, OT_NULL, sample_vproc_get_frame_send_to_venc_thread, (ot_void*)&send_attr);
    if (ret != OT_SUCCESS) {
        printf("create send frame thread failed!\n");
        g_send_chn_pthread = OT_FALSE;
        goto stop_venc;
    }

    g_save_file_pthread = OT_TRUE;
    ret = pthread_create(&save_pid, OT_NULL, sample_vproc_venc_get_venc_stream_proc, (ot_void*)dump_info);
    if (ret != OT_SUCCESS) {
        printf("create save frame thread failed!\n");
        g_save_file_pthread = OT_FALSE;
        goto stop_venc;
    }

    printf("threads running, print any key to exit!\n");
    getchar();

    g_save_file_pthread = OT_FALSE;
    g_send_chn_pthread = OT_FALSE;
    pthread_join(save_pid, OT_NULL);
    pthread_join(thread_id, OT_NULL);

stop_venc:
    ret = sample_comm_venc_stop(&g_sample_venc_attr);
    if (ret != OT_SUCCESS) {
        printf("sample_comm_venc_stop failed!\n");
    }
    return OT_NULL;
}

static ot_s32 dis_dump_get_config_file_name(dis_dump_info *dump_info,
    ot_char *ldc_file, ot_char *msensor_param_file)
{
    if (snprintf_s(ldc_file, FILE_NAME_LENGTH, FILE_NAME_LENGTH - 1, "./vi_pipe%d_chn%d_ldc.txt",
        dump_info->vi_pipe, dump_info->chn) == -1) {
        printf("set output  ldc_file name failed!\n");
        return OT_FAILURE;
    }
    if (snprintf_s(msensor_param_file, FILE_NAME_LENGTH, FILE_NAME_LENGTH - 1, "./vi_pipe%d_chn%d_msensor_param.txt",
        dump_info->vi_pipe, dump_info->chn) == -1) {
        printf("set output msensor_param_file name failed!\n");
        return OT_FAILURE;
    }
    return  OT_SUCCESS;
}

static ot_s32 dis_dump_get_data_file_name(dis_dump_info *dump_info,
    ot_char *acc, ot_char *gyro, ot_char *pts, ot_char *h_vmax)
{
    if (snprintf_s(pts, FILE_NAME_LENGTH, FILE_NAME_LENGTH - 1, "./vi_pipe%d_chn%d_pts_%d_data_%s.txt",
        dump_info->vi_pipe, dump_info->chn, dump_info->frame_cnt, date_str) == -1) {
        printf("set output pts_file name failed!\n");
        return OT_FAILURE;
    }

    if (snprintf_s(gyro, FILE_NAME_LENGTH, FILE_NAME_LENGTH - 1, "./vi_pipe%d_chn%d_gyro_%d_data_%s.txt",
        dump_info->vi_pipe, dump_info->chn, dump_info->frame_cnt, date_str) == -1) {
        printf("set output gyro_file name failed!\n");
        return OT_FAILURE;
    }

    if (snprintf_s(acc, FILE_NAME_LENGTH, FILE_NAME_LENGTH - 1, "./vi_pipe%d_chn%d_acc_%d_data_%s.txt",
        dump_info->vi_pipe, dump_info->chn, dump_info->frame_cnt, date_str) == -1) {
        printf("set output acc_file name failed!\n");
        return OT_FAILURE;
    }

    if (snprintf_s(h_vmax, FILE_NAME_LENGTH, FILE_NAME_LENGTH - 1, "./vi_pipe%d_chn%d_hmax_vmax_%d_data_%s.txt",
        dump_info->vi_pipe, dump_info->chn, dump_info->frame_cnt, date_str) == -1) {
        printf("set output hmax_vmax_file name failed!\n");
        return OT_FAILURE;
    }
    return  OT_SUCCESS;
}

static ot_s32 dis_dump_set_dump_info_msensor_buf(ot_msensor_param *msensor_param, dis_dump_info *dump_info)
{
    ot_msensor_buf_attr *msensor_buf = &dump_info->msensor_buf_attr;
    ot_void *virt_addr_tmp = OT_NULL;

    if (memcpy_s(msensor_buf, sizeof(ot_msensor_buf_attr), &msensor_param->buf_attr, sizeof(ot_msensor_buf_attr)) !=
        EOK) {
        printf("memcpy_s msensor_buf failed\n");
        return OT_FAILURE;
    }

    if (ot_smr_mmap(msensor_buf->phys_addr, msensor_buf->buf_len, OT_TRUE, &virt_addr_tmp) != 0) {
        printf("ot_smr_mmap (cached) failed!\n");
        return OT_FAILURE;
    }
    msensor_buf->virt_addr = (ot_u64)(ot_uintptr_t)virt_addr_tmp;

    return OT_SUCCESS;
}

static ot_s32 dis_dump_fopen_file(FILE **fd, ot_char *file_name)
{
    *fd = fopen(file_name, "wb");
    if (*fd == OT_NULL) {
        printf("open %s failed\n", file_name);
        return OT_FAILURE;
    }
    return OT_SUCCESS;
}

static ot_s32 dis_dump_open_gyro_config_file(ot_msensor_param *msensor_param, dis_dump_info *dump_info)
{
    ot_char ldc_file[FILE_NAME_LENGTH] = {0};
    ot_char msensor_param_file[FILE_NAME_LENGTH] = {0};

    ot_msensor_buf_attr *msensor_buf = &dump_info->msensor_buf_attr;

    if (dis_dump_get_config_file_name(dump_info, ldc_file, msensor_param_file) != OT_SUCCESS) {
        printf("dis_dump_get_config_file_name failed!\n");
        return OT_FAILURE;
    }

    if (dis_dump_set_dump_info_msensor_buf(msensor_param, dump_info) != OT_SUCCESS) {
        printf("dis_dump_set_dump_info_msensor_buf failed!\n");
        return OT_FAILURE;
    }

    if (dis_dump_fopen_file(&ldc_fd, ldc_file) != OT_SUCCESS) {
        printf("dis_dump_fopen_file failed!\n");
        goto exit1;
    }

    if (dis_dump_fopen_file(&msensor_param_fd, msensor_param_file) != OT_SUCCESS) {
        printf("dis_dump_fopen_file failed!\n");
        goto exit2;
    }

    (ot_void)fflush(stdout);

    return  OT_SUCCESS;

exit2:
    if (ldc_fd != OT_NULL) {
        fclose(ldc_fd);
        ldc_fd = OT_NULL;
    }
exit1:
    ot_smr_munmap((ot_void *)(ot_uintptr_t)msensor_buf->virt_addr, msensor_buf->buf_len);
    return OT_FAILURE;
}

static ot_s32 dis_dump_open_gyro_data_file(ot_msensor_param *msensor_param, dis_dump_info *dump_info)
{
    ot_char acc_file[FILE_NAME_LENGTH] = {0};
    ot_char gyro_file[FILE_NAME_LENGTH] = {0};
    ot_char pts_file[FILE_NAME_LENGTH] = {0};
    ot_char h_vmax_file[FILE_NAME_LENGTH] = {0};

    ot_msensor_buf_attr *msensor_buf = &dump_info->msensor_buf_attr;

    if (dis_dump_get_data_file_name(dump_info, acc_file, gyro_file, pts_file,
        h_vmax_file) != OT_SUCCESS) {
        return OT_FAILURE;
    }

    if (dis_dump_set_dump_info_msensor_buf(msensor_param, dump_info) != OT_SUCCESS) {
        return OT_FAILURE;
    }

    if (dis_dump_fopen_file(&dump_info->pts_fd, pts_file) != OT_SUCCESS) {
        goto exit1;
    }

    if (dis_dump_fopen_file(&dump_info->gyro_fd, gyro_file) != OT_SUCCESS) {
        goto exit2;
    }

    if (dis_dump_fopen_file(&dump_info->acc_fd, acc_file) != OT_SUCCESS) {
        goto exit3;
    }

    if (dis_dump_fopen_file(&dump_info->hv_fd, h_vmax_file) != OT_SUCCESS) {
        goto exit4;
    }

    (ot_void)fflush(stdout);

    return  OT_SUCCESS;

exit4:
    if (dump_info->acc_fd != OT_NULL) {
        fclose(dump_info->acc_fd);
        dump_info->acc_fd = OT_NULL;
    }
exit3:
    if (dump_info->gyro_fd != OT_NULL) {
        fclose(dump_info->gyro_fd);
        dump_info->gyro_fd = OT_NULL;
    }
exit2:
    if (dump_info->pts_fd != OT_NULL) {
        fclose(dump_info->pts_fd);
        dump_info->pts_fd = OT_NULL;
    }
exit1:
    ot_smr_munmap((ot_void *)(ot_uintptr_t)msensor_buf->virt_addr, msensor_buf->buf_len);
    return OT_FAILURE;
}

static ot_s32 dis_dump_get_input_num(ot_s32 *input_num)
{
    ot_s32 ret;
    ot_char buf[DIS_DUMP_ARG_BASE] = {0};

    (ot_void)fgets(buf, DIS_DUMP_ARG_BASE, stdin);
    ret = sscanf_s(buf, "%d", input_num);
    if (ret == -1) {
        printf("get input error");
        return OT_FAILURE;
    }
    return OT_SUCCESS;
}

static ot_s32 dis_dump_get_vproc_grp_chn(dis_dump_info *dump_info)
{
    ot_s32 ret;
    ot_s32 vproc_grp_tmp, vproc_chn_tmp;

    printf("This vi pipe(%d), dis is enable, need input vproc_grp vproc_chn\n", dump_info->vi_pipe);
    printf("please input vproc_grp: ");
    (ot_void)fflush(stdout);

    while (g_signal_flag == 0) {
        if (dis_dump_get_input_num(&vproc_grp_tmp) == OT_FAILURE) {
            continue;
        }
        if ((vproc_grp_tmp >= 0) && (vproc_grp_tmp < OT_VPROC_MAX_GRP_NUM)) {
            break;
        } else {
            printf("\nInvalid param, please enter again!\n\n");
            printf("please input vproc_grp: ");
        }
    }
    dump_info->vproc_grp = vproc_grp_tmp;

    printf("\nplease input vproc_chn: ");
    (ot_void)fflush(stdout);
    while (g_signal_flag == 0) {
        if (dis_dump_get_input_num(&vproc_chn_tmp) == OT_FAILURE) {
            continue;
        }
        if ((vproc_chn_tmp >= 0) && (vproc_chn_tmp < OT_VPROC_MAX_CHN_NUM)) {
            break;
        } else {
            printf("\nInvalid param, please enter again!\n\n");
            printf("please input vproc_chn: ");
        }
    }
    dump_info->vproc_chn = vproc_chn_tmp;

    ret = ot_eis_vproc_get_grp_hdl_by_grp_id(vproc_grp_tmp, &dump_info->vproc_grp_hdl);
    if (ret != OT_SUCCESS) {
        printf("ot_eis_vproc_get_grp_hdl_by_grp_id failed with %#x", ret);
    }

    ret = ot_eis_vproc_get_chn_hdl_by_ids(vproc_grp_tmp, vproc_chn_tmp, &dump_info->vproc_chn_hdl);
    if (ret != OT_SUCCESS) {
        printf("ot_eis_vproc_get_chn_hdl_by_ids failed with %#x", ret);
    }

    return ret;
}

static ot_s32 dis_dump_set_gyro_name(ot_msensor_param *msensor_param, dis_dump_info *dump_info0,
                                     dis_dump_info *dump_info1)
{
    ot_eis_dis_attr dis_attr0 = {0};
    ot_eis_dis_attr dis_attr1 = {0};
    ot_eis_dis_cfg dis_cfg0 = {0};

    if (ot_eis_vi_get_chn_dis_attr(dump_info0->vi_pipe_hdl, dump_info0->vi_chn_hdl, &dis_attr0) != OT_SUCCESS) {
        return OT_FAILURE;
    }
    if (ot_eis_vi_get_chn_dis_cfg(dump_info0->vi_pipe_hdl, dump_info0->vi_chn_hdl, &dis_cfg0) != OT_SUCCESS) {
        return OT_FAILURE;
    }
    dump_info0->dis_enable = dis_attr0.enable;
    dis_attr = dis_attr0;
    dis_cfg = dis_cfg0;

    if (ot_eis_vi_get_chn_dis_attr(dump_info1->vi_pipe_hdl, dump_info1->vi_chn_hdl, &dis_attr1) != OT_SUCCESS) {
        return OT_FAILURE;
    }
    dump_info1->dis_enable = dis_attr1.enable;

    if (dump_info0->enable == OT_TRUE) {
        if (dis_dump_get_vproc_grp_chn(dump_info0) != OT_SUCCESS) {
            printf("dump_info0: dis_dump_get_vproc_grp_chn failed!\n");
            return OT_FAILURE;
        }
        if (dis_dump_open_gyro_config_file(msensor_param, dump_info0) != OT_SUCCESS) {
            printf("dump_info0: dis_dump_open_gyro_config_file failed!\n");
            return OT_FAILURE;
        }
        if (dump_info0->dump_mode == DIS_DUMP_IMU_DATA || dump_info0->dump_mode == DIS_DUMP_STREAM_AND_IMU_DATA) {
            if (dis_dump_open_gyro_data_file(msensor_param, dump_info0) != OT_SUCCESS) {
                printf("dump_info0: dis_dump_open_gyro_file failed!\n");
                return OT_FAILURE;
            }
        }
    }

    if (dump_info1->enable == OT_TRUE) {
        if (dis_dump_get_vproc_grp_chn(dump_info1) != OT_SUCCESS) {
            printf("dump_info1: dis_dump_get_vproc_grp_chn failed!\n");
            return OT_FAILURE;
        }
        if (dis_dump_open_gyro_config_file(msensor_param, dump_info1) != OT_SUCCESS) {
                printf("dump_info1: dis_dump_open_gyro_config_file failed!\n");
                return OT_FAILURE;
        }
        if (dump_info1->dump_mode == DIS_DUMP_IMU_DATA || dump_info1->dump_mode == DIS_DUMP_STREAM_AND_IMU_DATA) {
            if (dis_dump_open_gyro_data_file(msensor_param, dump_info1) != OT_SUCCESS) {
                printf("dump_info1: dis_dump_open_gyro_file failed!\n");
                return OT_FAILURE;
            }
        }
    }

    return OT_SUCCESS;
}

static ot_void dis_dump_start_process(dis_dump_info *dump_info0, dis_dump_info *dump_info1)
{
    if (dump_info0->enable == OT_TRUE) {
        if (DUMP_YUV_MODE == 1) {
            pthread_create(&g_dis_dump_yuv_pid, NULL, dis_dump_start_get_yuv, dump_info0);
        }
        if (DUMP_STREAM_MODE == 1) {
            pthread_create(&g_dis_dump_stream_pid, NULL, dis_dump_start_get_stream, dump_info0);
        }
    }

    if (dump_info1->enable == OT_TRUE) {
        if (DUMP_YUV_MODE == 1) {
            pthread_create(&g_dis_dump_yuv_pid1, NULL, dis_dump_start_get_yuv, dump_info1);
        }
        if (DUMP_STREAM_MODE == 1) {
            pthread_create(&g_dis_dump_stream_pid1, NULL, dis_dump_start_get_stream, dump_info1);
        }
    }

    if (dump_info0->enable == OT_TRUE) {
        if (DUMP_YUV_MODE == 1) {
            pthread_join(g_dis_dump_yuv_pid, 0);
        }
        if (DUMP_STREAM_MODE == 1) {
            pthread_join(g_dis_dump_stream_pid, 0);
        }
    }
    if (dump_info1->enable == OT_TRUE) {
        if (DUMP_YUV_MODE == 1) {
            pthread_join(g_dis_dump_yuv_pid1, 0);
        }
        if (DUMP_STREAM_MODE == 1) {
            pthread_join(g_dis_dump_stream_pid1, 0);
        }
    }
}

static ot_void dis_dump_process(dis_dump_info *dump_info0, dis_dump_info *dump_info1)
{
    ot_msensor_param msensor_param = { 0 };
    ot_eis_mfusion_six_side_calibration six_side_calibration = { 0 };
    ot_eis_mfusion_drift gyro_drift = { 0 };
    ot_eis_mfusion_temperature_drift temperature_drift = { 0 };

    if (dis_dump_start_msensormng(&msensor_param) != OT_SUCCESS) {
        printf("dis_dump_start_msensormng failed\n");
        return;
    }

    if (dis_dump_mfusion_get_gyro_config(fusion_id, &six_side_calibration,
        &gyro_drift, &temperature_drift) != OT_SUCCESS) {
        printf("dis_dump_mfusion_get_gyro_config failed\n");
        goto exit0;
    }

    if (dis_dump_set_gyro_name(&msensor_param, dump_info0, dump_info1) != OT_SUCCESS) {
        printf("dis_dump_set_gyro_name failed\n");
        goto exit0;
    }

    if (dis_dump_get_mensor_param_config(&msensor_param, &six_side_calibration, &gyro_drift,
        &temperature_drift) != OT_SUCCESS) {
        printf("dis_dump_get_mensor_param faliure\n");
        goto exit0;
    }

    if (dis_dump_get_ldc_param_config(&dis_attr) != OT_SUCCESS) {
        printf("dis_dump_get_ldc_param_config faliure\n");
        goto exit0;
    }

    dis_dump_start_process(dump_info0, dump_info1);

exit0:
    dis_dump_stop_msensormng(dump_info0);
    if (dump_info1->enable == OT_TRUE) {
        dis_dump_stop_msensormng(dump_info1);
    }
    printf("exit success!!!\n");
}

static ot_s32 dis_dump_set_info(ot_vi_pipe vi_pipe, ot_s32 chn, ot_s32 frame_cnt, ot_s32 dump_mode,
                                dis_dump_info *dump_info)
{
    ot_s32 ret;
    dump_info->enable = OT_TRUE;

    if (!value_between(vi_pipe, 0, OT_VI_MAX_PIPE_NUM - 1)) {
        printf("pipe id must be [0,%d]!!!!\n\n", OT_VI_MAX_PIPE_NUM - 1);
        return OT_FAILURE;
    }
    dump_info->vi_pipe = vi_pipe;

    if (!value_between(dump_mode, 0, 2)) { /* min:0 max:2 */
        printf("dump_mode must be [0, 2]!!!!\n\n");
        return OT_FAILURE;
    }
    dump_info->dump_mode = (dis_dump_mode)dump_mode;

    if (DUMP_YUV_MODE == 1) {
        if (!value_between(chn, 0, OT_VI_MAX_CHN_NUM - 1)) {
            printf("chn id must be [0,%d]!!!!\n\n", OT_VI_MAX_CHN_NUM - 1);
            return OT_FAILURE;
        }
    }
    if (dump_mode == DIS_DUMP_STREAM)   {
        if (!value_between(chn, 0, OT_VENC_MAX_CHN_NUM - 1)) {
            printf("pipe id must be [0,%d]!!!!\n\n", OT_VENC_MAX_CHN_NUM - 1);
            return OT_FAILURE;
        }
    }

    dump_info->chn = chn;  //venc_chn_id
    if (frame_cnt < MIN_FRAMENUM) {
        printf("the frame cnt(%d) is invalid!\nthe frame cnt should be greater than 1!\n\n", frame_cnt);
        return OT_FAILURE;
    }
    dump_info->frame_cnt = frame_cnt;

    ret = ot_eis_vi_get_chn_hdl_by_ids(vi_pipe, chn, &dump_info->vi_chn_hdl);
    if (ret != OT_SUCCESS) {
        printf("ot_eis_vi_get_chn_hdl_by_ids failed with %#x", ret);
    }

    ret = ot_eis_vi_get_pipe_hdl_by_pipe_id(vi_pipe, &dump_info->vi_pipe_hdl);
    if (ret != OT_SUCCESS) {
        printf("ot_eis_vi_get_pipe_hdl_by_pipe_id failed with %#x", ret);
    }

    return OT_SUCCESS;
}

static ot_s32 dis_dump_check_argv(char *argv[], ot_s32 index, ot_s32 *val)
{
    ot_char *end_ptr = OT_NULL;
    ot_slong result;

    errno = 0;
    result = strtol(argv[index], &end_ptr, DIS_DUMP_ARG_BASE);
    if ((errno == ERANGE && (result == LONG_MAX || result == LONG_MIN)) || (errno != 0 && result == 0)) {
        return OT_FAILURE;
    }
    if ((end_ptr == argv[index]) || (*end_ptr != '\0')) {
        return OT_FAILURE;
    }

    *val = (ot_s32)result;

    return OT_SUCCESS;
}

//dump_info0:前4个参数，dump_info1:后4个参数
static ot_s32 dis_dump_read_args(dis_dump_info *dump_info0, dis_dump_info *dump_info1, char *argv[], int argc)
{
    ot_vi_pipe vi_pipe;
    ot_s32 chn;
    ot_s32 frame_cnt;
    ot_s32 dump_mode;

    if (dis_dump_check_argv(argv, 1, &vi_pipe) != OT_SUCCESS) {
        return OT_FAILURE;
    }

    if (dis_dump_check_argv(argv, 2, &chn) != OT_SUCCESS) { /* 2 args */  //venc_chn_id
        return OT_FAILURE;
    }

    if (dis_dump_check_argv(argv, 3, &frame_cnt) != OT_SUCCESS) { /* 3 args */
        return OT_FAILURE;
    }

    if (dis_dump_check_argv(argv, 4, &dump_mode) != OT_SUCCESS) { /* 4 args */
        return OT_FAILURE;
    }

    if (dis_dump_set_info(vi_pipe, chn, frame_cnt, dump_mode, dump_info0) != OT_SUCCESS) {
        return OT_FAILURE;
    }

    if (argc == 9) { /* 9 args */
            if (dis_dump_check_argv(argv, 5, &vi_pipe) != OT_SUCCESS) { /* 5 args */
                return OT_FAILURE;
            }

            if (dis_dump_check_argv(argv, 6, &chn) != OT_SUCCESS) { /* 6 args */
                return OT_FAILURE;
            }

            if (dis_dump_check_argv(argv, 7, &frame_cnt) != OT_SUCCESS) { /* 7 args */
                return OT_FAILURE;
            }

            if (dis_dump_check_argv(argv, 8, &dump_mode) != OT_SUCCESS) { /* 8 args */
                return OT_FAILURE;
            }
        if (dis_dump_set_info(vi_pipe, chn, frame_cnt, dump_mode, dump_info1) != OT_SUCCESS) {
            return OT_FAILURE;
        }
    }
    return OT_SUCCESS;
}

static ot_bool dis_dump_check_valid_digit_str(const ot_char *str)
{
    size_t i;
    size_t str_len;

    str_len = strlen(str);
    if (str_len > MAX_DIGIT_LEN) {
        return OT_FALSE;
    }

    for (i = 0; i < str_len; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return OT_FALSE;
        }
    }
    return OT_TRUE;
}

void dis_dump_handle_sig(int signo)
{
    if (g_signal_flag) {
        return;
    }

    if (signo == SIGINT || signo == SIGTERM) {
        g_signal_flag = 1;
    }
    return;
}

static ot_void usage(ot_void)
{
    printf("\n"
           "*************************************************\n"
           "Usage: ./dis_dump [pipe] [chn] [FrmCnt] [dump mode] \n"
           "       ./dis_dump [pipe] [chn] [FrmCnt] [dump mode]  [pipe] [chn] [FrmCnt] [dump mode]\n"
           "1)pipe: \n"
           "   vi pipe id\n"
           "2)chn: \n"
           "   chn is venc chn id, which will be created to dump stream\n"
           "3)FrmCnt: \n"
           "   the count of frame to be dump ,the value should be greater than 1\n"
           "4)dump mode: \n"
           "   dump mode == 0: dump stream\n"
           "   dump mode == 1: dump imu_data mode\n"
           "   dump mode == 2: dump stream and imu_data mode\n"
           "*)Example:\n"
           "   e.g : ./dis_dump 0 0 300 0\n"
           "   e.g : ./dis_dump 0 0 300 2 1 1 300 2\n"
           "*)set DIS_GET_CHN_FRAME_CONTINUOUSLY to 1 to get continuous frame\n"
           "*************************************************\n"
           "\n");
}


#ifdef __LITEOS__
ot_s32 dis_dump(int argc, char *argv[])
#else
ot_s32 main(int argc, char *argv[])
#endif
{
    ot_s32 i;
    dis_dump_info dump_info0 = {0};
    dis_dump_info dump_info1 = {0};
#ifndef __LITEOS__
    sig_t sig_handler;
#endif
    printf("\nNOTICE: This tool only can be used for dump imu data and yuv!\n");
    printf("\tTo see more usage, please enter: ./dis_dump -h\n\n");

    time(&now);
    timeinfo = localtime(&now);
    // 格式化日期和时间为字符串，例如：2023-10-27_15-30-00
    strftime(date_str, sizeof(date_str), "%Y-%m-%d_%H-%M-%S", timeinfo);

    if (argv == OT_NULL) {
        usage();
        return OT_SUCCESS;
    }
    if (argc > 1) {
        if (!strncmp(argv[1], "-h", 2)) { /* 2 help */
            usage();
            return OT_SUCCESS;
        }
    }
    if ((argc != 5) && (argc != 9)) { /* 5, 9 args */
        usage();
        return OT_SUCCESS;
    }

    for (i = 1; i < argc; i++) {
        if (!dis_dump_check_valid_digit_str(argv[i])) {
            printf("the %dth value :'%s' is invalid, must be reasonable non negative integers!!!!\n\n", i, argv[i]);
            usage();
            return -1;
        }
    }

    if (dis_dump_read_args(&dump_info0, &dump_info1, argv, argc) != OT_SUCCESS) {
        return -1;
    }

#ifndef __LITEOS__
    sig_handler = signal(SIGINT, dis_dump_handle_sig);
    if (sig_handler == SIG_ERR) {
        perror("error\n");
    }
    sig_handler = signal(SIGTERM, dis_dump_handle_sig);
    if (sig_handler == SIG_ERR) {
        perror("error\n");
    }
#endif

    dis_dump_process(&dump_info0, &dump_info1);

    return OT_SUCCESS;
}
