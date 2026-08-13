/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#include <unistd.h>
#include <pthread.h>
#include "sample_comm.h"
#include "ot_audio.h"
#include "ot_type.h"
#include "frame_cache.h"

#include <alsa/asoundlib.h>
#include "camera.h"

#ifndef ot_unused
#define ot_unused(x)             ((ot_void)(x))
#endif

#define sample_uac_debug(ret) \
    do { \
        printf("ret=%#x, fuc:%s, line:%d.\n", (ret), __FUNCTION__, __LINE__); \
    } while (0)

#define PCM_DEVICE_NAME "default"
#define UAC_SAMPLES_PER_FRAME 1024
#define PCM_WAIT_TIME_MS 1000

#define UAC_SAVE_FILE 0 /* dump usb audio frame and save to pcm, 0:off, 1:on */

#define UAC_PLAYBACK_SUPPORT 1 /* get audio from ai and send to usb, both linux and liteos support, 0:off, 1:on */
#ifndef __LITEOS__
#define UAC_CAPTURE_SUPPORT 1 /* get audio from usb and send to ao, only linux support, 0:off, 1:on */
#endif

typedef struct {
    ot_bool start;
    ot_s32  ai_dev;
    ot_s32  ai_chn;
    ot_aio_attr ai_attr;
    FILE *fd;
    pthread_t ai_pid;
} sample_ai_to_uac;

typedef struct {
    ot_bool start;
    ot_s32 ao_dev;
    ot_s32 ao_chn;
    ot_aio_attr ao_attr;
    FILE *fd;
    pthread_t ao_pid;
} sample_uac_to_ao;

typedef struct {
#ifndef __LITEOS__
    snd_pcm_uframes_t period_size;
#endif
    unsigned int channels;
} uac_playback_frame_info;

typedef struct {
    ot_aio_attr aio_attr;
    FILE *playback_fd;
    FILE *capture_fd;
    sample_comm_ai_vqe_param ai_vqe_param;
    ot_audio_dev ai_dev;
    ot_audio_dev ao_dev;
    ot_ai_chn ai_chn;
    ot_ao_chn ao_chn;
    ot_s32 ai_chn_cnt;
    ot_s32 ao_chn_cnt;
} uac_startup_param;

#ifndef __LITEOS__
#if UAC_PLAYBACK_SUPPORT
static snd_pcm_t *g_handle_playback = OT_NULL;
static snd_pcm_hw_params_t *g_params_playback = OT_NULL;
static snd_pcm_sw_params_t *g_sw_params_playback = OT_NULL;
#endif

#if UAC_CAPTURE_SUPPORT
static snd_pcm_t *g_handle_capture = OT_NULL;
static snd_pcm_hw_params_t *g_params_capture = OT_NULL;
#endif
#endif /* end of #ifndef __LITEOS__ */

#if UAC_PLAYBACK_SUPPORT
static ot_s32 g_ai_dev = -1;
static ot_s32 g_ai_chn = -1;
static ot_s32 g_ai_chn_cnt = -1;
static sample_ai_to_uac g_sample_ai_send_uac[OT_AI_DEV_MAX_NUM * OT_AI_MAX_CHN_NUM] = {0};
static ot_s16 g_playback_data[OT_MAX_AI_POINT_NUM * 2] = {0}; /* 2: 16bit */
#endif

#if UAC_CAPTURE_SUPPORT
static ot_s32 g_ao_dev = -1;
static ot_s32 g_ao_chn = -1;
static ot_s32 g_ao_chn_cnt = -1;
static sample_uac_to_ao g_sample_uac_send_ao[OT_AO_DEV_MAX_NUM * OT_AO_MAX_CHN_NUM] = {0};
static ot_s16 g_capture_data[OT_MAX_AO_POINT_NUM * 2] = {0}; /* 2: 16bit */
static ot_s16 g_ao_data[2][OT_MAX_AO_POINT_NUM] = {0}; /* 2: stereo */
#endif

static ot_bool g_aio_resample = OT_FALSE;
static ot_audio_sample_rate g_in_sample_rate = OT_AUDIO_SAMPLE_RATE_48000;
static ot_audio_sample_rate g_out_sample_rate = OT_AUDIO_SAMPLE_RATE_48000;

#if UAC_PLAYBACK_SUPPORT
#ifndef __LITEOS__
static ot_void interleave_16bit(ot_s16 *dest, ot_s16 *src_left, ot_s16 *src_right, ot_u32 samples)
{
    ot_u32 i;

    if ((dest == OT_NULL) || (src_left == OT_NULL) || (src_right == OT_NULL)) {
        return;
    }

    for (i = 0; i < samples; i++) {
        dest[2 * i] = *src_left; /* 2: 2chn */
        dest[2 * i + 1] = *src_right; /* 2: 2chn */
        src_left++;
        src_right++;
    }
}
#endif
#endif

#if UAC_CAPTURE_SUPPORT
static ot_void de_interleave_16bit(const ot_s16 *src, ot_s16 *dest_left, ot_s16 *dest_right, ot_u32 samples)
{
    ot_u32 i;

    if ((src == OT_NULL) || (dest_left == OT_NULL) || (dest_right == OT_NULL)) {
        return;
    }

    for (i = 0; i < samples; i++) {
        *dest_left = src[2 * i]; /* 2: 2chn */
        *dest_right = src[2 * i + 1]; /* 2: 2chn */
        dest_left++;
        dest_right++;
    }
}
#endif

#if UAC_SAVE_FILE
#if UAC_PLAYBACK_SUPPORT
static FILE *sample_uac_open_playback_file(ot_ai_chn ai_chn)
{
    FILE *fd;
    ot_s32 ret;
    ot_char file_name[FILE_NAME_LEN] = {0};

    /* create file for save stream */
#ifndef __LITEOS__
    ret = snprintf_s(file_name, FILE_NAME_LEN, FILE_NAME_LEN - 1, "uac_playback_chn%d.pcm", ai_chn);
    if (ret <= EOK) {
        printf("snprintf_s fail! ret = 0x%x\n", ret);
        return OT_NULL;
    }
#endif /* end of #ifdef __LITEOS__ */
    fd = fopen(file_name, "w+");
    if (fd == OT_NULL) {
        printf("%s: open file %s failed\n", __FUNCTION__, file_name);
        return OT_NULL;
    }
    printf("open stream file:\"%s\" for uac playback ok\n", file_name);
    return fd;
}
#endif /* end of #if UAC_PLAYBACK_SUPPORT */

#if UAC_CAPTURE_SUPPORT
static FILE *sample_uac_open_capture_file(ot_ao_chn ao_chn)
{
    FILE *fd = OT_NULL;
    ot_s32 ret;
    ot_char file_name[FILE_NAME_LEN] = {0};

    /* create file for save capture stream */
#ifndef __LITEOS__
    ret = snprintf_s(file_name, FILE_NAME_LEN, FILE_NAME_LEN - 1, "uac_capture_chn%d.pcm", ao_chn);
    if (ret <= EOK) {
        printf("snprintf_s fail! ret = 0x%x.\n", ret);
        return OT_NULL;
    }
#endif /* end of #ifdef __LITEOS__ */
    fd = fopen(file_name, "w+");
    if (fd == OT_NULL) {
        printf("%s: open file %s failed\n", __FUNCTION__, file_name);
        return OT_NULL;
    }
    printf("open stream file:\"%s\" for uac capture ok\n", file_name);
    return fd;
}
#endif  /* end of #if UAC_CAPTURE_SUPPORT */
#endif  /* end of #if UAC_SAVE_FILE */

#if UAC_PLAYBACK_SUPPORT
#ifndef __LITEOS__
static ot_s32 send_frame_to_alsa_pre_proc(const ot_audio_frame *frame_org, ot_audio_frame *audio_frame,
    snd_pcm_uframes_t frames, unsigned int channels)
{
    int err;

    if (frame_org->bit_width != OT_AUDIO_BIT_WIDTH_16) {
        printf("%s: bit_width is not 16 bits.\n", __FUNCTION__);
        return OT_FAILURE;
    }

    err = memcpy_s(audio_frame, sizeof(*audio_frame), frame_org, sizeof(*frame_org));
    if (err != EOK) {
        printf("memcpy_s fail! err = 0x%x.\n", err);
        return OT_FAILURE;
    }

    /* copy data to buf */
    if (channels == 1) {
        size_t size = frames * sizeof(ot_s16);
        err = memcpy_s(g_playback_data, size, audio_frame->virt_addr[0], audio_frame->len);
        if (err != EOK) {
            printf("memcpy_s fail! err = 0x%x.\n", err);
            return OT_FAILURE;
        }
    } else if (channels == 2) { /* 2 chn */
        interleave_16bit(g_playback_data, (ot_s16 *)(audio_frame->virt_addr[0]),
            (ot_s16 *)(audio_frame->virt_addr[1]), audio_frame->len / sizeof(ot_s16));
    } else {
        printf("%s: channels is invalid.\n", __FUNCTION__);
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}

static ot_s32 sample_uac_send_frame_to_alsa(const ot_audio_frame *frame, snd_pcm_uframes_t period_size,
    unsigned int channels)
{
    ot_audio_frame audio_frame;
    int err;
    ot_bool send_finish_status = OT_FALSE;

    err = send_frame_to_alsa_pre_proc(frame, &audio_frame, period_size, channels);
    if (err != OT_SUCCESS) {
        return OT_FAILURE;
    }

    /* send data to alsa driver */
    while (send_finish_status == OT_FALSE) {
        if (uvc_apps_get_quit_flag() != 0) {
            break;
        }

        err = snd_pcm_wait(g_handle_playback, PCM_WAIT_TIME_MS);
        if (err == 0) {
            continue;
        }

        err = snd_pcm_writei(g_handle_playback, g_playback_data, period_size);
        if (err == -EPIPE) {
            /* EPIPE means underrun */
            printf("underrun occurred, err = %d\n", err);
            snd_pcm_prepare(g_handle_playback);
        } else if (err < 0) {
            printf("error from writei: %s\n", snd_strerror(err));
            break;
        } else if (err != (int)period_size) {
            printf("short write, write %d frames\n", err);
            break;
        } else {
            send_finish_status = OT_TRUE;
        }
    }

    return OT_SUCCESS;
}
#endif /* end of #ifndef __LITEOS__ */
#endif /* end of #if UAC_PLAYBACK_SUPPORT */

#if UAC_PLAYBACK_SUPPORT
#ifndef __LITEOS__
static ot_s32 alsa_playback_set_sw_param(const sample_ai_to_uac *ai_uac_ctrl)
{
    int err;
    unsigned int val;

    /* get sw_params */
    err = snd_pcm_sw_params_current(g_handle_playback, g_sw_params_playback);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    /* start_threshold */
    val = ai_uac_ctrl->ai_attr.point_num_per_frame + 1; /* start after at least 2 frame */
    err = snd_pcm_sw_params_set_start_threshold(g_handle_playback, g_sw_params_playback, val);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    /* set the sw_params to the driver */
    err = snd_pcm_sw_params(g_handle_playback, g_sw_params_playback);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}

static ot_s32 alsa_playback_set_hw_param(const sample_ai_to_uac *ai_uac_ctrl)
{
    int err;
    int dir = SND_PCM_STREAM_PLAYBACK;
    unsigned int val;

    /* Interleaved mode */
    err = snd_pcm_hw_params_set_access(g_handle_playback, g_params_playback, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    /* Signed 16-bit little-endian format */
    err = snd_pcm_hw_params_set_format(g_handle_playback, g_params_playback, SND_PCM_FORMAT_S16_LE);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    /* channels */
    val = ai_uac_ctrl->ai_attr.snd_mode + 1; /* mono:1, stereo:2 */
    err = snd_pcm_hw_params_set_channels(g_handle_playback, g_params_playback, val);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    /* period (default: 1024) */
    val = ai_uac_ctrl->ai_attr.point_num_per_frame;
    err = snd_pcm_hw_params_set_period_size(g_handle_playback, g_params_playback, val, dir);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    /* buf size */
    val = ai_uac_ctrl->ai_attr.point_num_per_frame * 4; /* 4: frame num */
    err = snd_pcm_hw_params_set_buffer_size(g_handle_playback, g_params_playback, val);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    /* sampling rate */
    val = ai_uac_ctrl->ai_attr.sample_rate;
    err = snd_pcm_hw_params_set_rate_near(g_handle_playback, g_params_playback, &val, &dir);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    /* Write the parameters to the driver */
    err = snd_pcm_hw_params(g_handle_playback, g_params_playback);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}

static ot_s32 alsa_playback_get_period_and_channel(snd_pcm_uframes_t *period_size, unsigned int *channels)
{
    int err, dir;

    err = snd_pcm_hw_params_get_period_size(g_params_playback, period_size, &dir);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s\n", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    } else if ((err > 0) && (*period_size == 0)) {
        *period_size = err;
    }

    err = snd_pcm_hw_params_get_channels(g_params_playback, channels);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s\n", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}

static ot_s32 alsa_playback_init(const sample_ai_to_uac *ai_uac_ctrl, snd_pcm_uframes_t *period_size,
    unsigned int *channels)
{
    /* alsa */
    int err;

    /* Open PCM device for playback. */
    err = snd_pcm_open(&g_handle_playback, PCM_DEVICE_NAME, SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        printf("audio open error: %s\n", snd_strerror(err));
        return OT_FAILURE;
    }

    err = snd_pcm_hw_params_malloc(&g_params_playback);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    err = snd_pcm_sw_params_malloc(&g_sw_params_playback);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    /* Fill it in with default values. */
    err = snd_pcm_hw_params_any(g_handle_playback, g_params_playback);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    if (alsa_playback_set_hw_param(ai_uac_ctrl) != OT_SUCCESS) {
        printf("[Func]:%s [Line]:%d fail to set hw_param\n", __FUNCTION__, __LINE__);
        return OT_FAILURE;
    }

    if (alsa_playback_set_sw_param(ai_uac_ctrl) != OT_SUCCESS) {
        printf("[Func]:%s [Line]:%d fail to set sw_param\n", __FUNCTION__, __LINE__);
        return OT_FAILURE;
    }

    if (alsa_playback_get_period_and_channel(period_size, channels) != OT_SUCCESS) {
        printf("[Func]:%s [Line]:%d fail to get period_and_chnnel\n", __FUNCTION__, __LINE__);
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}
#endif /* end of #ifndef __LITEOS__ */

static ot_s32 sample_uac_set_ai_chn_and_get_fd(const sample_ai_to_uac *ai_uac_ctrl, ot_s32 *ai_chn_fd)
{
    ot_s32 ret;
    ot_ai_chn_param ai_chn_param;

    ret = ss_mpi_ai_get_chn_param(ai_uac_ctrl->ai_dev, ai_uac_ctrl->ai_chn, &ai_chn_param);
    if (ret != OT_SUCCESS) {
        printf("%s: Get ai chn param failed\n", __FUNCTION__);
        return OT_FAILURE;
    }

    ai_chn_param.usr_frame_depth = 5; /* 5: frame depth */

    ret = ss_mpi_ai_set_chn_param(ai_uac_ctrl->ai_dev, ai_uac_ctrl->ai_chn, &ai_chn_param);
    if (ret != OT_SUCCESS) {
        printf("%s: set ai chn param failed\n", __FUNCTION__);
        return OT_FAILURE;
    }

    *ai_chn_fd = ss_mpi_ai_get_fd(ai_uac_ctrl->ai_dev, ai_uac_ctrl->ai_chn);

    return OT_SUCCESS;
}

static ot_s32 uac_playback_proc_select(ot_s32 ai_chn_fd, fd_set *read_fds)
{
    ot_s32 ret;
    struct timeval timeout_val;

    timeout_val.tv_sec = 1;
    timeout_val.tv_usec = 0;

    FD_ZERO(read_fds);
    FD_SET(ai_chn_fd, read_fds);

    ret = select(ai_chn_fd + 1, read_fds, OT_NULL, OT_NULL, &timeout_val);
    if (ret < 0) {
        printf("%s: get ai frame select fail.\n", __FUNCTION__);
        return OT_FAILURE;
    } else if (ret == 0) {
        printf("%s: get ai frame select time out.\n", __FUNCTION__);
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}

static ot_void uac_playback_proc_dump(const sample_ai_to_uac *ai_uac_ctrl, ot_audio_frame *frame)
{
#if UAC_SAVE_FILE
    ot_s32 total_size = (frame->snd_mode == OT_AUDIO_SOUND_MODE_MONO) ?
        frame->len : (frame->len * 2); /* 2:stereo */
    (ot_void)fwrite(g_playback_data, 1, total_size, ai_uac_ctrl->fd);
    (ot_void)fflush(ai_uac_ctrl->fd);
#else
    ot_unused(ai_uac_ctrl);
    ot_unused(frame);
#endif
}

static ot_s32 uac_playback_proc_core(const sample_ai_to_uac *ai_uac_ctrl,
    const uac_playback_frame_info *play_frame_info)
{
    ot_s32 ret;
    ot_audio_frame audio_frame = { 0 };
    ot_aec_frame aec_frame = { 0 };

    /* get frame from ai chn */
    ret = ss_mpi_ai_get_frame(ai_uac_ctrl->ai_dev, ai_uac_ctrl->ai_chn, &audio_frame, &aec_frame, OT_FALSE);
    if (ret != OT_SUCCESS) {
        return OT_SUCCESS;
    }

    /* send frame to uac */
#ifndef __LITEOS__
    ret = sample_uac_send_frame_to_alsa(&audio_frame, play_frame_info->period_size, play_frame_info->channels);
#else
    ot_unused(play_frame_info);
#endif
    if (ret != OT_SUCCESS) {
        printf("%s: send frame failed with %#x!\n", __FUNCTION__, ret);
        return ret;
    }

    /* save data to file */
    uac_playback_proc_dump(ai_uac_ctrl, &audio_frame);

    /* release frame */
    ret = ss_mpi_ai_release_frame(ai_uac_ctrl->ai_dev, ai_uac_ctrl->ai_chn, &audio_frame, &aec_frame);
    if (ret != OT_SUCCESS) {
        printf("LINE:%d, %s: ai release frame(%d, %d), failed with %#x!\n", __LINE__, __FUNCTION__,
            ai_uac_ctrl->ai_dev, ai_uac_ctrl->ai_chn, ret);
        return ret;
    }

    return OT_SUCCESS;
}

static ot_void uac_playback_proc_exit(ot_void)
{
#ifndef __LITEOS__
    if (g_sw_params_playback != OT_NULL) {
        snd_pcm_sw_params_free(g_sw_params_playback);
        g_sw_params_playback = OT_NULL;
    }

    if (g_params_playback != OT_NULL) {
        snd_pcm_hw_params_free(g_params_playback);
        g_params_playback = OT_NULL;
    }

    if (g_handle_playback != OT_NULL) {
        snd_pcm_close(g_handle_playback);
        g_handle_playback = OT_NULL;
    }
#endif
}

/* function : get frame from ai, send it to uac */
void *sample_uac_uac_playback_proc(void *parg)
{
    ot_s32 ret, ai_chn_fd;
    sample_ai_to_uac *ai_uac_ctrl = (sample_ai_to_uac *)parg;
    fd_set read_fds;
    uac_playback_frame_info play_frame_info = {0};

#ifndef __LITEOS__
    ret = alsa_playback_init(ai_uac_ctrl, &(play_frame_info.period_size), &(play_frame_info.channels));
    if (ret != OT_SUCCESS) {
        goto bail;
    }
#endif

    ret = sample_uac_set_ai_chn_and_get_fd(ai_uac_ctrl, &ai_chn_fd);
    if (ret != OT_SUCCESS) {
        goto bail;
    }

    while (ai_uac_ctrl->start) {
#ifndef __LITEOS__
        if (uvc_apps_get_quit_flag() != 0) {
            break;
        }
#endif
        if (uac_playback_proc_select(ai_chn_fd, &read_fds) != OT_SUCCESS) {
            break;
        }

        if (FD_ISSET(ai_chn_fd, &read_fds)) {
            ret = uac_playback_proc_core(ai_uac_ctrl, &play_frame_info);
            if (ret != OT_SUCCESS) {
                break;
            }
        }
    }

bail:
    ai_uac_ctrl->start = OT_FALSE;
    uac_playback_proc_exit();
    return OT_NULL;
}
#endif /* end of #if UAC_PLAYBACK_SUPPORT */

#if UAC_CAPTURE_SUPPORT
static ot_s32 alsa_capture_set_param(const sample_uac_to_ao *uac_ao_ctrl)
{
    int err;
    unsigned int val;
    const int dir = 0;

    /* Interleaved mode */
    err = snd_pcm_hw_params_set_access(g_handle_capture, g_params_capture, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    /* Signed 16-bit little-endian format */
    err = snd_pcm_hw_params_set_format(g_handle_capture, g_params_capture, SND_PCM_FORMAT_S16_LE);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    /* channels */
    val = uac_ao_ctrl->ao_attr.snd_mode + 1; /* mono:1, stereo:2 */
    err = snd_pcm_hw_params_set_channels(g_handle_capture, g_params_capture, val);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    /* period (default: 1024) */
    val = uac_ao_ctrl->ao_attr.point_num_per_frame;
    err = snd_pcm_hw_params_set_period_size(g_handle_capture, g_params_capture, val, dir);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    /* buf size */
    val = uac_ao_ctrl->ao_attr.point_num_per_frame * 4; /* 4: frame num */
    err = snd_pcm_hw_params_set_buffer_size(g_handle_capture, g_params_capture, val);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    /* sampling rate */
    val = uac_ao_ctrl->ao_attr.sample_rate;
    err = snd_pcm_hw_params_set_rate(g_handle_capture, g_params_capture, val, dir);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    /* Write the parameters to the driver */
    err = snd_pcm_hw_params(g_handle_capture, g_params_capture);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}

static ot_s32 alsa_capture_get_period_and_channel(snd_pcm_uframes_t *period_size, unsigned int *channels)
{
    int err, dir;

    /* check if frame size is right */
    err = snd_pcm_hw_params_get_period_size(g_params_capture, period_size, &dir);
    if (err > 0 && *period_size == 0) {
        *period_size = err;
    }

    err = snd_pcm_hw_params_get_channels(g_params_capture, channels);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s.\n", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}

static ot_s32 alsa_capture_init(const sample_uac_to_ao *uac_ao_ctrl, snd_pcm_uframes_t *period_size,
    unsigned int *channels)
{
    /* alsa */
    int err, dir;
    unsigned int val;

    /* Open PCM device for capture. */
    err = snd_pcm_open(&g_handle_capture, PCM_DEVICE_NAME, SND_PCM_STREAM_CAPTURE, 0);
    if (err < 0) {
        printf("audio open error: %s\n", snd_strerror(err));
        return OT_FAILURE;
    }

    err = snd_pcm_hw_params_malloc(&g_params_capture);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    /* Fill it in with default values. */
    err = snd_pcm_hw_params_any(g_handle_capture, g_params_capture);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    if (alsa_capture_set_param(uac_ao_ctrl) != OT_SUCCESS) {
        return OT_FAILURE;
    }

    if (alsa_capture_get_period_and_channel(period_size, channels) != OT_SUCCESS) {
        return OT_FAILURE;
    }

    /* check if samplerate is right */
    err = snd_pcm_hw_params_get_rate(g_params_capture, &val, &dir);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s.\n", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }
    if (val != uac_ao_ctrl->ao_attr.sample_rate) {
        printf("val = %u is not match samplerate = %d.\n", val, uac_ao_ctrl->ao_attr.sample_rate);
        return OT_FAILURE;
    }

    err = snd_pcm_start(g_handle_capture);
    if (err < 0) {
        printf("[Func]:%s [Line]:%d info error: %s.\n", __FUNCTION__, __LINE__, snd_strerror(err));
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}

static ot_s32 uac_capture_set_ao_frame(ot_audio_frame *ao_frame, unsigned int channels, snd_pcm_uframes_t period_size)
{
    ot_s32 ret;

    ao_frame->bit_width = OT_AUDIO_BIT_WIDTH_16;
    ao_frame->virt_addr[0] = (ot_u8 *)&(g_ao_data[0][0]);
    if (channels == 1) { /* 1: mono */
        ao_frame->snd_mode = OT_AUDIO_SOUND_MODE_MONO;
    } else if (channels == 2) { /* 2: stereo */
        ao_frame->snd_mode = OT_AUDIO_SOUND_MODE_STEREO;
        ao_frame->virt_addr[1] = (ot_u8 *)&(g_ao_data[1][0]);
    } else {
        printf("channels = %u is wrong\n", channels);
        return OT_FAILURE;
    }

    ao_frame->len = period_size * sizeof(ot_s16);

    if (ao_frame->snd_mode == OT_AUDIO_SOUND_MODE_STEREO) {
        de_interleave_16bit((ot_s16 *)g_capture_data, (ot_s16 *)(ao_frame->virt_addr[0]),
            (ot_s16 *)(ao_frame->virt_addr[1]), ao_frame->len / sizeof(ot_s16));
    } else {
        ret = memcpy_s(ao_frame->virt_addr[0], OT_MAX_AO_POINT_NUM * sizeof(ot_s16), g_capture_data, ao_frame->len);
        if (ret != EOK) {
            printf("ao_frame.virt_addr[0] memcpy_s fail, ret:0x%x\n", ret);
            return OT_FAILURE;
        }
    }

    return OT_SUCCESS;
}

static ot_s32 uac_capture_proc_core(const sample_uac_to_ao *uac_ao_ctrl, unsigned int channels,
    snd_pcm_uframes_t period_size)
{
    ot_s32 ret;
    int err;
    ot_audio_frame ao_frame = { 0 };

    err = snd_pcm_wait(g_handle_capture, PCM_WAIT_TIME_MS);
    if (err == 0) {
        return OT_SUCCESS;
    }

    err = snd_pcm_readi(g_handle_capture, g_capture_data, period_size);
    if (err == -EPIPE) {
        /* EPIPE means overrun */
        printf("overrun occurred, err = %d.\n", err);
        snd_pcm_prepare(g_handle_capture);
        snd_pcm_start(g_handle_capture);
        return OT_SUCCESS;
    } else if (err < 0) {
        printf("error from readi: %s.\n", snd_strerror(err));
        return OT_SUCCESS;
    } else if (err != (int)period_size) {
        printf("error read, readi %d period_size.\n", err);
        return OT_SUCCESS;
    }

    /* save data to file */
#if UAC_SAVE_FILE
    (ot_void)fwrite(g_capture_data, 1, period_size * channels * sizeof(ot_s16), uac_ao_ctrl->fd);
    (ot_void)fflush(uac_ao_ctrl->fd);
#endif

    ret = uac_capture_set_ao_frame(&ao_frame, channels, period_size);
    if (ret != OT_SUCCESS) {
        return ret;
    }

    /* send frame to ao */
    ret = ss_mpi_ao_send_frame(uac_ao_ctrl->ao_dev, uac_ao_ctrl->ao_chn, &ao_frame, 1000); /* 1000ms timeout */
    if (ret != OT_SUCCESS) {
        printf("%s: ao send frame failed with %#x!\n", __FUNCTION__, ret);
        return OT_SUCCESS;
    }

    return OT_SUCCESS;
}

static ot_void uac_capture_proc_exit(ot_void)
{
    if (g_params_capture != OT_NULL) {
        snd_pcm_hw_params_free(g_params_capture);
        g_params_capture = OT_NULL;
    }

    if (g_handle_capture != OT_NULL) {
        snd_pcm_close(g_handle_capture);
        g_handle_capture = OT_NULL;
    }
}

/* function : get frame from uac, send it to ao */
void *sample_uac_uac_capture_proc(void *arg)
{
    ot_s32 ret;
    sample_uac_to_ao *uac_ao_ctrl = (sample_uac_to_ao *)arg;
    unsigned int channels = 0;
    snd_pcm_uframes_t period_size = 0;

    ret = alsa_capture_init(uac_ao_ctrl, &period_size, &channels);
    if (ret != OT_SUCCESS) {
        goto fail;
    }

    while (uac_ao_ctrl->start) {
#ifndef __LITEOS__
        if (uvc_apps_get_quit_flag() != 0) {
            break;
        }
#endif

        ret = uac_capture_proc_core(uac_ao_ctrl, channels, period_size);
        if (ret != OT_SUCCESS) {
            break;
        }
    }

fail:
    uac_ao_ctrl->start = OT_FALSE;
    uac_capture_proc_exit();

    return OT_NULL;
}
#endif /* end of #if UAC_CAPTURE_SUPPORT */

#if UAC_PLAYBACK_SUPPORT
/* function : Create the thread to get frame from ai and send to uac */
ot_s32 sample_uac_create_trd_uac_playback(ot_audio_dev ai_dev,
    ot_ai_chn ai_chn, ot_aio_attr ai_attr, FILE *playback_fd)
{
    sample_ai_to_uac *ai_uac_ctrl = OT_NULL;

    ai_uac_ctrl = &g_sample_ai_send_uac[ai_dev * OT_AI_MAX_CHN_NUM + ai_chn];
    ai_uac_ctrl->ai_dev = ai_dev;
    ai_uac_ctrl->ai_chn = ai_chn;
    ai_uac_ctrl->ai_attr = ai_attr;
    ai_uac_ctrl->start = OT_TRUE;
    ai_uac_ctrl->fd = playback_fd;

    pthread_create(&ai_uac_ctrl->ai_pid, 0, sample_uac_uac_playback_proc, ai_uac_ctrl);

    return OT_SUCCESS;
}

/* function : Destroy the thread to get frame from ai and send to uac */
static ot_void sample_uac_destroy_trd_uac_playback(ot_audio_dev ai_dev, ot_ai_chn ai_chn)
{
    sample_ai_to_uac *ai_uac_ctrl = OT_NULL;

    ai_uac_ctrl = &g_sample_ai_send_uac[ai_dev * OT_AI_MAX_CHN_NUM + ai_chn];
    if (ai_uac_ctrl->start) {
        ai_uac_ctrl->start = OT_FALSE;
        pthread_join(ai_uac_ctrl->ai_pid, 0);
    }

    if (ai_uac_ctrl->fd != OT_NULL) {
        fclose(ai_uac_ctrl->fd);
        ai_uac_ctrl->fd = OT_NULL;
    }
}
#endif /* end of #if UAC_PLAYBACK_SUPPORT */

#if UAC_CAPTURE_SUPPORT
/* function : Create the thread to get frame from uac and send to ao */
static ot_s32 sample_uac_create_trd_uac_capture(ot_audio_dev ao_dev,
    ot_ao_chn ao_chn, ot_aio_attr ao_attr, FILE *capture_fd)
{
    sample_uac_to_ao *uac_ao_ctrl = OT_NULL;

    uac_ao_ctrl = &g_sample_uac_send_ao[ao_dev * OT_AO_MAX_CHN_NUM + ao_chn];
    uac_ao_ctrl->ao_dev = ao_dev;
    uac_ao_ctrl->ao_chn = ao_chn;
    uac_ao_ctrl->ao_attr = ao_attr;
    uac_ao_ctrl->start = OT_TRUE;
    uac_ao_ctrl->fd = capture_fd;

    pthread_create(&uac_ao_ctrl->ao_pid, 0, sample_uac_uac_capture_proc, uac_ao_ctrl);

    return OT_SUCCESS;
}

/* function : Destroy the thread to get frame from uac and send to ao */
static ot_void sample_uac_destroy_trd_uac_capture(ot_audio_dev ao_dev, ot_ao_chn ao_chn)
{
    sample_uac_to_ao *uac_ao_ctrl = OT_NULL;

    uac_ao_ctrl = &g_sample_uac_send_ao[ao_dev * OT_AO_MAX_CHN_NUM + ao_chn];
    if (uac_ao_ctrl->start == OT_TRUE) {
        uac_ao_ctrl->start = OT_FALSE;
        pthread_join(uac_ao_ctrl->ao_pid, 0);
    }

    if (uac_ao_ctrl->fd != OT_NULL) {
        fclose(uac_ao_ctrl->fd);
        uac_ao_ctrl->fd = OT_NULL;
    }
}
#endif /* end of #if UAC_CAPTURE_SUPPORT */

static ot_s32 sample_uac_init(ot_void)
{
    ot_s32 ret;

    ss_mpi_audio_exit();

    ret = ss_mpi_audio_init();
    if (ret != OT_SUCCESS) {
        sample_print("ss_mpi_audio_init failed!\n");
        return OT_FAILURE;
    }

    return ret;
}

static ot_void sample_uac_init_attr(ot_aio_attr *aio_attr)
{
    aio_attr->sample_rate   = g_in_sample_rate;
    aio_attr->bit_width     = OT_AUDIO_BIT_WIDTH_16;
    aio_attr->work_mode     = OT_AIO_MODE_I2S_MASTER;
    aio_attr->snd_mode      = OT_AUDIO_SOUND_MODE_MONO;
    aio_attr->expand_flag   = 0;
    aio_attr->frame_num     = 5; /* 5: frame num */
    aio_attr->point_num_per_frame = UAC_SAMPLES_PER_FRAME;
    aio_attr->chn_cnt       = 1;
    aio_attr->clk_share     = 1; /* ai need to share ao clock for inner codec */
    aio_attr->i2s_type      = OT_AIO_I2STYPE_INNERCODEC;
}

static ot_void sample_uac_set_ai_vqe_param(sample_comm_ai_vqe_param *ai_vqe_param,
    ot_audio_sample_rate out_sample_rate, ot_bool resample_en, ot_void *ai_vqe_attr, ot_u32 ai_vqe_type)
{
    ai_vqe_param->out_sample_rate = out_sample_rate;
    ai_vqe_param->resample_en = resample_en;
    ai_vqe_param->ai_vqe_attr = ai_vqe_attr;
    ai_vqe_param->ai_vqe_type = ai_vqe_type;
}

static ot_s32 sample_uac_open_dump_file(FILE **playback_fd, FILE **capture_fd,
    ot_ai_chn ai_chn, ot_ao_chn ao_chn)
{
#if UAC_SAVE_FILE
#if UAC_PLAYBACK_SUPPORT
    /* open uac playback file */
    *playback_fd = sample_uac_open_playback_file(ai_chn);
    if (*playback_fd == OT_NULL) {
        sample_uac_debug(OT_FAILURE);
        return OT_FAILURE;
    }
#endif

#if UAC_CAPTURE_SUPPORT
    /* open uac capture file */
    *capture_fd = sample_uac_open_capture_file(ao_chn);
    if (*capture_fd == OT_NULL) {
        sample_uac_debug(OT_FAILURE);
        return OT_FAILURE;
    }
#endif
#endif

    return OT_SUCCESS;
}

static ot_void sample_uac_close_dump_file(FILE *playback_fd, FILE *capture_fd)
{
    if (capture_fd != OT_NULL) {
        fclose(capture_fd);
    }

    if (playback_fd != OT_NULL) {
        fclose(playback_fd);
    }
}

static ot_s32 sample_uac_startup_init(uac_startup_param *param)
{
    param->playback_fd = OT_NULL;
    param->capture_fd = OT_NULL;

    param->ai_dev = sample_uac_INNER_AI_DEV;
    param->ao_dev = sample_uac_INNER_AO_DEV;

    param->ai_chn = 0;
    param->ao_chn = 0;

    if (sample_uac_open_dump_file(&(param->playback_fd), &(param->capture_fd),
        param->ai_chn, param->ao_chn) != OT_SUCCESS) {
        sample_uac_close_dump_file(param->playback_fd, param->capture_fd);
        param->playback_fd = OT_NULL;
        param->capture_fd = OT_NULL;
        return OT_FAILURE;
    }

    sample_uac_init_attr(&(param->aio_attr));

    sample_uac_set_ai_vqe_param(&(param->ai_vqe_param), g_out_sample_rate, g_aio_resample, OT_NULL, 0);

    param->ai_chn_cnt = param->aio_attr.chn_cnt;
    param->ao_chn_cnt = param->aio_attr.chn_cnt;

    return OT_SUCCESS;
}

static ot_void sample_uac_startup_exit(uac_startup_param *param)
{
    sample_uac_close_dump_file(param->playback_fd, param->capture_fd);
    param->playback_fd = OT_NULL;
    param->capture_fd = OT_NULL;
}

static ot_s32 sample_uac_startup_ai(uac_startup_param *param)
{
#if UAC_PLAYBACK_SUPPORT
    /* enable AI channel */
    return sample_comm_audio_start_ai(param->ai_dev, param->ai_chn_cnt,
        &(param->aio_attr), &(param->ai_vqe_param), -1);
#else
    ot_unused(param);
    return OT_SUCCESS;
#endif
}

static ot_void sample_uac_stop_ai(const uac_startup_param *param)
{
#if UAC_PLAYBACK_SUPPORT
    ot_s32 ret = sample_comm_audio_stop_ai(param->ai_dev, param->ai_chn_cnt, g_aio_resample, OT_FALSE);
    if (ret != OT_SUCCESS) {
        sample_uac_debug(ret);
    }
#else
    ot_unused(param);
#endif
}

static ot_s32 sample_uac_startup_ao(uac_startup_param *param)
{
#if UAC_CAPTURE_SUPPORT
    /* enable AO channel */
    return sample_comm_audio_start_ao(param->ao_dev, param->ao_chn_cnt, &(param->aio_attr),
        g_in_sample_rate, g_aio_resample);
#else
    ot_unused(param);
    return OT_SUCCESS;
#endif
}

static ot_void sample_uac_stop_ao(const uac_startup_param *param)
{
#if UAC_CAPTURE_SUPPORT
    ot_s32 ret = sample_comm_audio_stop_ao(param->ao_dev, param->ao_chn_cnt, g_aio_resample);
    if (ret != OT_SUCCESS) {
        sample_uac_debug(ret);
    }
#else
    ot_unused(param);
#endif
}

static ot_s32 sample_uac_start_ai_ao(uac_startup_param *param)
{
    ot_s32 ret;

    ret = sample_uac_startup_ai(param);
    if (ret != OT_SUCCESS) {
        sample_uac_debug(ret);
        goto aiao_err3;
    }

    ret = sample_uac_startup_ao(param);
    if (ret != OT_SUCCESS) {
        sample_uac_debug(ret);
        goto aiao_err2;
    }

    /* config internal audio codec */
    ret = sample_comm_audio_cfg_acodec(&(param->aio_attr));
    if (ret != OT_SUCCESS) {
        sample_uac_debug(ret);
        goto aiao_err1;
    }

    return OT_SUCCESS;

aiao_err1:
    sample_uac_stop_ao(param);

aiao_err2:
    sample_uac_stop_ai(param);

aiao_err3:
    return ret;
}

static ot_void sample_uac_stop_ai_ao(const uac_startup_param *param)
{
    sample_uac_stop_ao(param);

    sample_uac_stop_ai(param);

    return;
}

static ot_s32 sample_uac_create_uac_send_thread(const uac_startup_param *param)
{
#if UAC_PLAYBACK_SUPPORT
    return sample_uac_create_trd_uac_playback(param->ai_dev, param->ai_chn, param->aio_attr, param->playback_fd);
#else
    ot_unused(param);
    return OT_SUCCESS;
#endif
}

static ot_s32 sample_uac_create_uac_receive_thread(const uac_startup_param *param)
{
#if UAC_CAPTURE_SUPPORT
    return sample_uac_create_trd_uac_capture(param->ao_dev, param->ao_chn, param->aio_attr, param->capture_fd);
#else
    ot_unused(param);
    return OT_SUCCESS;
#endif
}

static ot_void sample_uac_destroy_uac_send_thread(const uac_startup_param *param)
{
#if UAC_PLAYBACK_SUPPORT
    sample_uac_destroy_trd_uac_playback(param->ai_dev, param->ai_chn);
#else
    ot_unused(param);
#endif
}

static ot_s32 sample_uac_create_thread(const uac_startup_param *param)
{
    ot_s32 ret;

    /* create uac send thread */
    ret = sample_uac_create_uac_send_thread(param);
    if (ret != OT_SUCCESS) {
        sample_uac_debug(ret);
        goto thread_err2;
    }

    /* create uac receive thread */
    ret = sample_uac_create_uac_receive_thread(param);
    if (ret != OT_SUCCESS) {
        sample_uac_debug(ret);
        goto thread_err1;
    }

    return OT_SUCCESS;

thread_err1:
    sample_uac_destroy_uac_send_thread(param);

thread_err2:
    return ret;
}

static ot_void sample_uac_startup_save_param(const uac_startup_param *param)
{
#if UAC_PLAYBACK_SUPPORT
    g_ai_dev = param->ai_dev;
    g_ai_chn = param->ai_chn;
    g_ai_chn_cnt = param->ai_chn_cnt;
#endif

#if UAC_CAPTURE_SUPPORT
    g_ao_dev = param->ao_dev;
    g_ao_chn = param->ao_chn;
    g_ao_chn_cnt = param->ao_chn_cnt;
#endif
}

static ot_s32 sample_uac_startup(ot_void)
{
    ot_s32 ret;
    uac_startup_param startup_param = {0};

    ret = sample_uac_startup_init(&startup_param);
    if (ret != OT_SUCCESS) {
        goto start_err3;
    }

    ret = sample_uac_start_ai_ao(&startup_param);
    if (ret != OT_SUCCESS) {
        sample_uac_debug(ret);
        goto start_err2;
    }

    ret = sample_uac_create_thread(&startup_param);
    if (ret != OT_SUCCESS) {
        sample_uac_debug(ret);
        goto start_err1;
    }

    sample_uac_startup_save_param(&startup_param);
    return OT_SUCCESS;

start_err1:
    sample_uac_stop_ai_ao(&startup_param);

start_err2:
    sample_uac_startup_exit(&startup_param);

start_err3:
    return ret;
}

static ot_s32 sample_uac_shutdown(ot_void)
{
    ot_s32 ret;

#if UAC_PLAYBACK_SUPPORT
    sample_uac_destroy_trd_uac_playback(g_ai_dev, g_ai_chn);

    uac_playback_proc_exit();

    ret = sample_comm_audio_stop_ai(g_ai_dev, g_ai_chn_cnt, g_aio_resample, OT_FALSE);
    if (ret != OT_SUCCESS) {
        sample_uac_debug(ret);
    }
#endif

#if UAC_CAPTURE_SUPPORT
    sample_uac_destroy_trd_uac_capture(g_ao_dev, g_ao_chn);

    uac_capture_proc_exit();

    ret = sample_comm_audio_stop_ao(g_ao_dev, g_ao_chn_cnt, g_aio_resample);
    if (ret != OT_SUCCESS) {
        sample_uac_debug(ret);
    }
#endif

    return ret;
}

static struct audio_control_ops audio_sc_ops = {
    .init = sample_uac_init,
    .startup = sample_uac_startup,
    .shutdown = sample_uac_shutdown,
};

ot_void camera_register_uac_mpi_ops(ot_void)
{
    ot_audio_register_mpi_ops(&audio_sc_ops);
}
