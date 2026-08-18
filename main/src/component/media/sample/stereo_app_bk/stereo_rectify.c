/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.

  Stereo rectification via VPROC GDC 2D XY-LUT.
  LUT files are generated from stereo_calib.json offline and contain the full
  undistort + stereoRectify mapping in native 1080x1280 sensor space.
 */

#include <stdio.h>
#include <string.h>
#include "stereo_rectify.h"
#include "stereo_types.h"
#include "sample_comm.h"
#include "ot_eis_vproc_grp.h"

extern ot_void sample_comm_vproc_free_2dlut(ot_eis_vproc_xylut_param *xylut);

#define RECTIFY_LOG(fmt, ...) \
    stereo_log_write("[stereo_rectify] " fmt, ##__VA_ARGS__)

#define STEREO_XYLUT_NATIVE_W  1080
#define STEREO_XYLUT_NATIVE_H  1280

static ot_bool                    g_xylut_enabled = OT_FALSE;
static ot_eis_vproc_gdc_cfg       g_gdc_cfg[2];
static ot_eis_vproc_xylut_param   g_xylut_param[2];
static ot_eis_vproc_gdc_xylut     g_xylut_lut[2];

static void free_xylut_configs(void)
{
    for (int i = 0; i < 2; i++) {
        if (g_xylut_param[i].gdc_lut != OT_NULL &&
            g_xylut_param[i].gdc_lut->gdc_2dlut_data != 0) {
            sample_comm_vproc_free_2dlut(&g_xylut_param[i]);
        }
        memset(&g_gdc_cfg[i], 0, sizeof(g_gdc_cfg[i]));
        memset(&g_xylut_param[i], 0, sizeof(g_xylut_param[i]));
        memset(&g_xylut_lut[i], 0, sizeof(g_xylut_lut[i]));
    }
    g_xylut_enabled = OT_FALSE;
}

static void build_xylut_gdc_seed(ot_eis_vproc_gdc_cfg *gdc, ot_u32 src_w, ot_u32 src_h)
{
    memset(gdc, 0, sizeof(*gdc));
    gdc->ldc_or_fisheye = OT_EIS_GDC_LDC_SEL;
    gdc->gdc_mode       = OT_EIS_WORK_2D_LUT_POLAT;
    gdc->src.width      = src_w;
    gdc->src.height     = src_h;
    gdc->mid.width      = src_w;
    gdc->mid.height     = src_h;
    gdc->dst.width      = src_w;
    gdc->dst.height     = src_h;
    gdc->cell_size      = OT_EIS_LUT_CELL_SIZE_16;
}

ot_s32 stereo_rectify_load_xylut(ot_u32 src_width, ot_u32 src_height)
{
    const char *lut_paths[2] = {STEREO_LUT_LEFT_PATH, STEREO_LUT_RIGHT_PATH};
    const char *labels[2] = {"left", "right"};

    if (src_width != STEREO_XYLUT_NATIVE_W || src_height != STEREO_XYLUT_NATIVE_H) {
        RECTIFY_LOG("ERR: unsupported XY-LUT native=%ux%u, only native=%ux%u is supported\n",
                    src_width, src_height, STEREO_XYLUT_NATIVE_W, STEREO_XYLUT_NATIVE_H);
        return OT_FAILURE;
    }

    free_xylut_configs();
    RECTIFY_LOG("loading XY-LUT files: %s, %s (src=%ux%u)\n",
                STEREO_LUT_LEFT_PATH, STEREO_LUT_RIGHT_PATH, src_width, src_height);

    for (int i = 0; i < 2; i++) {
        ot_s32 ret;
        ot_eis_vproc_gdc_param gdc_param;

        build_xylut_gdc_seed(&g_gdc_cfg[i], src_width, src_height);
        memset(&g_xylut_lut[i], 0, sizeof(g_xylut_lut[i]));
        memset(&g_xylut_param[i], 0, sizeof(g_xylut_param[i]));
        g_xylut_param[i].gdc_lut = &g_xylut_lut[i];

        memset(&gdc_param, 0, sizeof(gdc_param));
        gdc_param.enable = OT_TRUE;
        gdc_param.cfg = &g_gdc_cfg[i];

        ret = sample_comm_vproc_load_gdc_2dlut_file((ot_s8 *)lut_paths[i], &gdc_param, &g_xylut_param[i]);
        if (ret != OT_SUCCESS) {
            RECTIFY_LOG("ERR: %s load XY-LUT failed: %s ret=0x%x\n", labels[i], lut_paths[i], ret);
            free_xylut_configs();
            return ret;
        }

        RECTIFY_LOG("%s XY-LUT loaded: src=%ux%u dst=%ux%u mode=%d cell=%d phys=0x%llx len=%u\n",
                    labels[i], g_gdc_cfg[i].src.width, g_gdc_cfg[i].src.height,
                    g_gdc_cfg[i].dst.width, g_gdc_cfg[i].dst.height,
                    g_gdc_cfg[i].gdc_mode, g_gdc_cfg[i].cell_size,
                    (unsigned long long)g_xylut_lut[i].gdc_2dlut_data,
                    g_xylut_lut[i].xylut_len);
    }

    g_xylut_enabled = OT_TRUE;
    RECTIFY_LOG("XY-LUT stereo rectification ready for both cameras\n");
    return OT_SUCCESS;
}

ot_bool stereo_rectify_is_xylut_enabled(void)
{
    return g_xylut_enabled;
}

ot_eis_vproc_gdc_cfg *stereo_rectify_get_gdc_cfg(int cam_idx)
{
    if (cam_idx < STEREO_CAM_LEFT || cam_idx > STEREO_CAM_RIGHT) {
        return OT_NULL;
    }
    if (g_xylut_enabled != OT_TRUE) {
        return OT_NULL;
    }
    return &g_gdc_cfg[cam_idx];
}

ot_eis_vproc_xylut_param *stereo_rectify_get_xylut_param(int cam_idx)
{
    if (cam_idx < STEREO_CAM_LEFT || cam_idx > STEREO_CAM_RIGHT) {
        return OT_NULL;
    }
    if (g_xylut_enabled != OT_TRUE) {
        return OT_NULL;
    }
    return &g_xylut_param[cam_idx];
}

ot_s32 stereo_rectify_apply_xylut(ot_eis_handle left_grp_hdl, ot_eis_handle right_grp_hdl)
{
    if (g_xylut_enabled != OT_TRUE) {
        RECTIFY_LOG("ERR: XY-LUT not loaded\n");
        return OT_FAILURE;
    }

    ot_eis_handle grp_hdls[2] = {left_grp_hdl, right_grp_hdl};
    const char *labels[2] = {"left", "right"};

    for (int i = 0; i < 2; i++) {
        ot_s32 ret;
        ot_eis_vproc_gdc_param gdc_param;
        memset(&gdc_param, 0, sizeof(gdc_param));
        gdc_param.enable = OT_TRUE;
        gdc_param.cfg = &g_gdc_cfg[i];

        ret = ot_eis_vproc_grp_set_gdc(grp_hdls[i], &gdc_param);
        if (ret != OT_SUCCESS) {
            RECTIFY_LOG("ERR: %s set_gdc(XY-LUT) failed: 0x%x\n", labels[i], ret);
            return ret;
        }
        RECTIFY_LOG("%s set_gdc(XY-LUT) OK\n", labels[i]);

        ret = ot_eis_vproc_grp_set_xylut(grp_hdls[i], &g_xylut_param[i]);
        if (ret != OT_SUCCESS) {
            RECTIFY_LOG("ERR: %s set_xylut failed: 0x%x\n", labels[i], ret);
            return ret;
        }
        RECTIFY_LOG("%s set_xylut OK\n", labels[i]);
    }

    RECTIFY_LOG("XY-LUT stereo rectification applied\n");
    return OT_SUCCESS;
}

void stereo_rectify_deinit(void)
{
    free_xylut_configs();
    RECTIFY_LOG("XY-LUT rectification disabled\n");
}
