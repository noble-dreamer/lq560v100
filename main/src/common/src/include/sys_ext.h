/*
  Copyright (c), 2021-2023, Oritek Tech. Co., Ltd.
 */

#ifndef SYS_EXT_H
#define SYS_EXT_H

#include "ot_type.h"
#include "mod_ext.h"
#include "ot_common_sys.h"
#include "ot_common_sys_bind.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

#define SYS_DDR_MAXNUM                16
#define SYS_PAGE_SHIFT       12


#define SYS_SEND_DATA_BLOCK_MASK      0x1
#define SYS_SEND_DATA_BLOCK_OFFSET    0
#define SYS_SEND_DATA_NOBLOCK         ((0 << SYS_SEND_DATA_BLOCK_OFFSET) & SYS_SEND_DATA_BLOCK_MASK)
#define SYS_SEND_DATA_BLOCK           ((1 << SYS_SEND_DATA_BLOCK_OFFSET) & SYS_SEND_DATA_BLOCK_MASK)

#define SYS_SEND_DATA_DELAY_MASK      0x6
#define SYS_SEND_DATA_DELAY_OFFSET    1
#define SYS_SEND_DATA_FINISH          ((0 << SYS_SEND_DATA_DELAY_OFFSET) & SYS_SEND_DATA_DELAY_MASK)
#define SYS_SEND_DATA_LOWDELAY        ((1 << SYS_SEND_DATA_DELAY_OFFSET) & SYS_SEND_DATA_DELAY_MASK)
#define SYS_SEND_DATA_LOWDELAY_FINISH ((2 << SYS_SEND_DATA_DELAY_OFFSET) & SYS_SEND_DATA_DELAY_MASK)

typedef struct {
    ot_u32 num;
    ot_void *smr_name[SYS_DDR_MAXNUM];
} omp_sys_ddr_name;

typedef enum {
    OMP_DATA_VI_FRAME,
    OMP_DATA_VOU_FRAME,
    OMP_DATA_VDEC_FRAME,
    OMP_DATA_VIDEO_FRAME,
    OMP_DATA_VOU_WBC_FRAME,
    OMP_DATA_AUDIO_FRAME,
    OMP_DATA_VPROC_FRAME,
    OMP_DATA_DPU_RECT_FRAME,
    OMP_DATA_AVS_FRAME,
    OMP_DATA_MCF_FRAME,
    OMP_DATA_VENC_STREAM,
    OMP_DATA_BUTT
} omp_data_type;

typedef struct {
    ot_mod_id mod_id;
    ot_u32 max_dev_cnt;
    ot_u32 max_chn_cnt;
    ot_s32 (*give_bind_call_back)(ot_s32 dev_id, ot_s32 chn_id, ot_omp_bind_dst *bind_send);
} bind_sender_info;

typedef struct {
    ot_mod_id mod_id;
    ot_u32 max_dev_cnt;
    ot_u32 max_chn_cnt;
    ot_s32 (*call_back)(ot_s32 dev_id, ot_s32 chn_id, ot_bool block, omp_data_type data_type, ot_void *pv_data);
    ot_s32 (*reset_call_back)(ot_s32 dev_id, ot_s32 chn_id, ot_void *pv_data);
    ot_bool support_delay_data;
} bind_receiver_info;

typedef enum {
    SYS_AI_SAMPLECLK_DIVSEL,
    SYS_AI_BITCKL_DIVSEL,
    SYS_AI_SYSCKL_SEL,
    SYS_AI_MASTER,
    SYS_AI_RESET_SEL,
    SYS_AI_CLK_EN,

    SYS_AO_SAMPLECLK_DIVSEL,
    SYS_AO_BITCKL_DIVSEL,
    SYS_AO_SYSCKL_SEL,
    SYS_AO_MASTER,
    SYS_AO_RESET_SEL,
    SYS_AO_CLK_EN,

    SYS_AIO_RESET_SEL,
    SYS_AIO_CLK_EN,
    SYS_AIO_CLK_SEL,

    SYS_AIO_RX0SD_SEL,
    SYS_AIO_HDMISD_SEL,
    SYS_AIO_PADSD_SEL,
    SYS_AIO_CODECSD_SEL,
    SYS_AIO_HDMICLK_SEL,
    SYS_AIO_PADCLK_SEL,
    SYS_AIO_CODECCLK_SEL,

    SYS_ACODEC_RESET_SEL,

    SYS_DMIC_RESET_SEL,
    SYS_DMIC_CLK_EN,

    SYS_VI_PT_INPUT_SEL,
    SYS_VI_RESET_SEL,
    SYS_VI_BUS_CLK_EN,
    SYS_VI_PPC_CLK_EN,
    SYS_VI_DEV_RESET_SEL,
    SYS_VI_DEV_CLK_EN,
    SYS_VI_CMOS_CLK_DOUBLE,
    SYS_VI_CMOS_CLK_EN,
    SYS_VI_CMOS_RESET_SEL,
    SYS_VI_APB_CLK_EN,
    SYS_VI_DLL_EN,
    SYS_VI_THERMO_WORK_MODE_SEL,
    SYS_VI_IR_CLK_GET,
    SYS_VI_BUS_RESET,
    SYS_VI_PPC_CLK_GET,
    SYS_VI_PPC_RESET,
    SYS_VI_PPC_PT_CLK_EN,
    SYS_VI_PPC_CHN_CLK_EN,
    SYS_VI_ISP_CLK_EN,
    SYS_VI_ISP_CORE_RESET,
    SYS_VI_ISP_CFG_RESET,
    SYS_VI_ISP_CLK_GET,
    SYS_VI_PT_CLK_EN,
    SYS_VI_PT_RESET,
    SYS_VI_PROC_PRE_CLK_EN,
    SYS_VI_PROC_PRE_RESET,
    SYS_VI_PROC_CLK_SET,
    SYS_VI_PROC_CLK_GET,
    SYS_VI_CAP_PROC_PRE_ONLINE_SEL,
    SYS_VI_PROC_POST_VPROC_ONLINE_SEL,
    SYS_VI_SYS_CNT_GET,

    SYS_VO_PLL_POSTDIV2_SET,
    SYS_VO_PLL_POSTDIV2_GET,
    SYS_VO_PLL_POSTDIV1_SET,
    SYS_VO_PLL_POSTDIV1_GET,
    SYS_VO_PLL_FRAC_SET,
    SYS_VO_PLL_FRAC_GET,
    SYS_VO_PLL_REFDIV_SET,
    SYS_VO_PLL_FBDIV_SET,
    SYS_VO_PLL_FBDIV_GET,
    SYS_VO_PLL_POWER_CTRL,
    SYS_VO_BUS_RESET_SEL,
    SYS_VO_CFG_CLK_EN,
    SYS_VO_APB_CLK_EN,
    SYS_VO_APB_GET_CLK_EN,
    SYS_VO_BUS_CLK_EN,
    SYS_VO_HD_CLKOUT_PHASIC_REVERSE_EN,
    SYS_VO_OUT_DLY_TUNE,
    SYS_VO_OUT_HD_DIV,
    SYS_VO_DEV_DIV_MOD,
    SYS_VO_PPC_CFG_CLK_SEL,
    SYS_VO_PPC_CLK_SEL,
    SYS_VO_HD_CLK_SEL,
    SYS_VO_HD_GET_CLK_SEL,
    SYS_VO_CORE_CLK_EN,
    SYS_VO_DEV_CLK_EN,
    SYS_VO_DEV_DATE_CLK_EN,
    SYS_VO_SD_DATE_CLK_EN,
    SYS_VO_SD_CLK_SEL,
    SYS_VO_LCD_CLK_EN,
    SYS_VO_LCD_MCLK_DIV,
    SYS_VO_GET_LCD_MCLK_DIV,
    SYS_VO_BT_RGB_CLK_EN,
    SYS_VO_BT_RGB_CLK_SEL,
    SYS_VO_DAC_CLK_PHASIC_REVERSE_EN,
    SYS_VO_HD_DAC_SEL,
    SYS_VO_DEV_DAC_EN,
    SYS_VO_HDMI_SSC_VDP_DIV,
    SYS_VO_HDMI_CLK_EN,
    SYS_VO_HDMI_CLK_SEL,
    SYS_VO_HDMI1_CLK_EN,
    SYS_VO_HDMI1_CLK_SEL,
    SYS_VO_MIPI_CLK_EN,
    SYS_VO_MIPI_CLK_SEL,

    SYS_VEDU_RESET_SEL,
    SYS_VEDU_CLK_EN,
    SYS_VEDU_CLK_SEL,
    SYS_VEDU_GET_CLK_SEL,
    SYS_VEDU_SED_RESET_SEL,
    SYS_VEDU_SED_CLK_SEL,
    SYS_VEDU_SED_CLK_EN,
    SYS_VEDU_PWR_EN,
    SYS_VEDU_PWR_ISO,
    SYS_VEDU_PWR_STATE,
    SYS_VEDU_BPLL_CFG0_SET,
    SYS_VEDU_BPLL_CFG1_SET,

    SYS_SCD_RESET_SEL,
    SYS_SCD_CLK_EN,

    SYS_VPROC_CLK_SEL,
    SYS_VPROC_RESET_SEL,
    SYS_VPROC_CLK_EN,
    SYS_VPROC_GET_CLK_SEL,
    SYS_VPROC_SET_CLK_SEL,
    SYS_AVS_RESET_SEL,
    SYS_AVS_CLK_EN,
    SYS_AVS_PWR_EN,
    SYS_AVS_PWR_ISO,
    SYS_AVS_PWR_STATE,

    SYS_OGE_RESET_SEL,
    SYS_OGE_CLK_EN,
    SYS_OGE_SET_CLK_SEL,
    SYS_OGE_GET_CLK_SEL,

    SYS_JPGE_RESET_SEL,
    SYS_JPGE_CLK_EN,
    SYS_JPGE_CLK_SEL,

    SYS_JPGD_CLOCK_SEL,
    SYS_JPGD_RESET_SEL,
    SYS_JPGD_CLK_EN,
    SYS_JPGD_RIGHT_EAR_CLK_EN,

    SYS_MD_RESET_SEL,
    SYS_MD_CLK_EN,
    SYS_MD_CLK_SEL,

    SYS_CVE_RESET_SEL,
    SYS_CVE_CLK_EN,
    SYS_CVE_CLK_SET,
    SYS_CVE_CLK_GET,
    SYS_CVE_SRC_CLK_EN,

    SYS_FD_RESET_SEL,
    SYS_FD_CLK_EN,

    SYS_WK_CNN_RESET_SEL,
    SYS_WK_CNN_CLK_EN,
    SYS_WK_CNN_CLK_SET,
    SYS_WK_CNN_CLK_GET,

    SYS_MAU_CLK_SET,
    SYS_MAU_RESET_SEL,
    SYS_MAU_CLK_EN,
    SYS_MAU_CLK_GET,

    SYS_DPU_CLK_SET,
    SYS_DPU_RESET_SEL,
    SYS_DPU_CLK_EN,

    SYS_PQP_RESET_SEL,
    SYS_PQP_CLK_EN,
    SYS_PQP_CLK_SET,
    SYS_PQP_CLK_GET,

    SYS_AVP_NPU0_RESET_SEL,
    SYS_AVP_NPU0_RESET_STA,
    SYS_AVP_NPU0_CLK_EN,
    SYS_AVP_NPU0_CLK_SET,
    SYS_AVP_NPU0_CLK_GET,

    SYS_PCIE_RESET_SEL,
    SYS_PCIE_CLK_EN,

    SYS_CIPHER_RESET_SEL,
    SYS_CIPHER_CLK_EN,

    SYS_VGS_RESET_SEL,
    SYS_VGS_CLK_EN,
    SYS_VGS_GET_CLK_SEL,
    SYS_VGS_SET_CLK_SEL,
    SYS_VGS_BOOTROOM_RAM_USE,

    SYS_GDC_RESET_SEL,
    SYS_GDC_CLK_EN,
    SYS_GDC_GET_CLK_STATE,
    SYS_GDC_NPU_RAM_USE,
    SYS_GDC_GET_CLKSEL,
    SYS_GDC_SET_CLKSEL,

    SYS_GDC_VGS_PWR_EN,
    SYS_GDC_VGS_PWR_ISO,
    SYS_GDC_VGS_PWR_STATE,

    SYS_VPROC_PWR_EN,
    SYS_VPROC_PWR_ISO,
    SYS_VPROC_PWR_STATE,

    SYS_DIS_RESET_SEL,
    SYS_DIS_CLK_EN,

    SYS_GDC_NPU_MUTEX_SEL,
    SYS_VENC_NPU_MUTEX_SEL,
    SYS_NPU_GET_MUTEX_STATE,
    SYS_NPU_GDC_GET_MUTEX_STATE,

    SYS_GET_SYS,
    SYS_GET_SCT,

    SYS_KCF0_RESET_SEL,
    SYS_KCF0_CLK_EN,
    SYS_KCF0_CLK_SET,
    SYS_KCF0_CLK_GET,

    SYS_KCF1_RESET_SEL,
    SYS_KCF1_CLK_EN,
    SYS_KCF1_CLK_SET,
    SYS_KCF1_CLK_GET,

    SYS_DSP_CLK_SET,
    SYS_DSP_CORE_RESET_SEL,
    SYS_DSP_CFG_RESET_SEL,
    SYS_DSP_DBG_RESET_SEL,
    SYS_DSP_JTAG_RESET_SEL,
    SYS_DSP_CLK_EN,

    SYS_NPU_SRST_RESET,
    SYS_NPU_DIV_SRST_RESET,
    SYS_NPU_CLK_EN,
    SYS_NPU_CLK_SEL,
    SYS_NPU_CLK_GET,
    SYS_NPU_SW_BYPASS_CKSEL,
    SYS_NPU_CLK_SDIV,
    SYS_NPU_CPM_CKEN,
    SYS_CHECK_APLL_CLK,

    SYS_HDMI0_CTRL_RESET_SEL,
    SYS_HDMI0_PHY_RESET_SEL,
    SYS_HDMI1_CTRL_RESET_SEL,
    SYS_HDMI1_PHY_RESET_SEL,

    SYS_CFG_LOCK_MUX,
    SYS_CFG_UNLOCK_MUX,
    SYS_RESET_LOCK_MUX,
    SYS_RESET_UNLOCK_MUX,

    SYS_BUTT,
} sys_func;

#ifdef CONFIG_OT_TZASC_SUPPORT
typedef enum {
    NON_TEE_VERSION = 0,
    TEE_VERSION = 1,
} sys_security_version;
#endif

typedef struct {
    ot_u64 (*pfn_sys_get_time_stamp)(ot_void);
    ot_u64 (*pfn_sys_get_local_time_stamp)(ot_void);
    ot_void (*pfn_sys_sync_time_stamp)(ot_u64 base, ot_bool init);
    ot_u32 (*pfn_sys_get_chip_version)(ot_void);
    ot_s32 (*pfn_sys_get_stride)(ot_u32 width, ot_u32 *stride);
    ot_s32 (*pfn_sys_drv_ioctrl)(const ot_omp_chn *omp_chn, sys_func func_id, ot_void *io_args);

    ot_s32 (*pfn_sys_register_sender)(const bind_sender_info *info);
    ot_s32 (*pfn_sys_unregister_sender)(ot_mod_id mod_id);
    ot_s32 (*pfn_sys_register_receiver)(const bind_receiver_info *info);
    ot_s32 (*pfn_sys_unregister_receiver)(ot_mod_id mod_id);
    ot_s32 (*pfn_sys_send_data)(ot_omp_chn *bind_chn, ot_u32 flag, omp_data_type data_type, ot_void *pv_data);
    ot_s32 (*pfn_sys_reset_data)(ot_mod_id mod_id, ot_s32 dev_id, ot_s32 chn_id, ot_void *private);

    ot_s32 (*pfn_get_bind_by_src)(const ot_omp_chn *src_chn, ot_omp_bind_dst *bind_src);
    ot_s32 (*pfn_get_bind_num_by_src)(const ot_omp_chn *src_chn, ot_u32 *bind_num);
    ot_s32 (*pfn_get_bind_by_dest)(const ot_omp_chn *dest_chn, ot_omp_chn *src_chn);
    ot_s32 (*pfn_get_smr_name)(const ot_omp_chn *chn, ot_void **pp_smr_name);
    ot_s32 (*pfn_get_mem_ddr)(const ot_omp_chn *chn, omp_sys_ddr_name *mem_ddr);
    ot_s32 (*pfn_get_spin_rec)(ot_u32 *spin_rec);

    ot_u32 (*pfn_sys_vreg_read)(ot_u32 addr, ot_u32 bytes);
    ot_s32 (*pfn_sys_vreg_write)(ot_u32 addr, ot_u32 value, ot_u32 bytes);
    ot_u32 (*pfn_sys_get_vreg_addr)(ot_void);
    ot_u64 (*pfn_sys_get_sched_clock)(ot_void);
    ot_s32 (*pfn_sys_get_full_chip_id)(ot_u32 *chip_id);

    ot_u32 (*pfn_sys_get_chip_id)(ot_void);
    ot_phys_addr_t (*pfn_sys_get_efuse_addr)(ot_void);
    ot_phys_addr_t (*pfn_sys_get_sys_base_addr)(ot_void);

    ot_s32 (*pfn_sys_get_time_zone)(ot_s32 *time_zone);
    ot_s32 (*pfn_sys_get_now_string)(ot_u8 *now_time);
    ot_gps_info *(*pfn_sys_get_gps_info)(ot_void);

    ot_void (*pfn_sys_get_dci_coef) (ot_phys_addr_t *phy_addr);

    ot_s32 (*pfn_sys_print_proc_title)(osal_proc_entry *s, const ot_char *format, ...) \
        __attribute__((format(printf, 2, 3)));

    ot_bool (*pfn_sys_is_tee_enable_otp)(ot_void);

#ifdef CONFIG_OT_TZASC_SUPPORT
    ot_s32 (*pfn_sys_get_security_version)(sys_security_version *version);
#endif
#ifdef CONFIG_OT_VI_PTS
    ot_u64 (*pfn_sys_get_vi_time_stamp)(ot_s32 dev_id);
#endif
#ifdef CONFIG_OT_SYS_FMU_SUPPORT
    ot_s32 (*pfn_sys_get_fmu_page_num)(ot_fmu_id id, ot_u32 *page_num);
#endif
    ot_s32 (*pfn_sys_get_vi_vproc_mode)(ot_vi_vproc_mode *mode);
    ot_u32 (*pfn_sys_get_mipirx_ext_cfg)(ot_void);
} sys_export_func;

#define ckfn_sys_entry() check_func_entry(OT_ID_SYS)

#define ckfn_sys_get_sched_clock() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_sched_clock != OT_NULL)
#define call_sys_get_sched_clock() \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_sched_clock()

#define ckfn_sys_get_time_stamp() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_time_stamp != OT_NULL)
#define call_sys_get_time_stamp() \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_time_stamp()

#ifdef CONFIG_OT_VI_PTS
#define ckfn_sys_get_vi_time_stamp() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_vi_time_stamp != OT_NULL)
#define call_sys_get_vi_time_stamp(dev_id) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_vi_time_stamp(dev_id)
#endif

#define ckfn_sys_get_local_time_stamp() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_local_time_stamp != OT_NULL)
#define call_sys_get_local_time_stamp() \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_local_time_stamp()

#define ckfn_sys_sync_time_stamp() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_sync_time_stamp != OT_NULL)
#define call_sys_sync_time_stamp(base, init) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_sync_time_stamp(base, init)

#define ckfn_sys_get_stride() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_stride != OT_NULL)
#define call_sys_get_stride(width, stride) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_stride(width, stride)

#define ckfn_sys_get_chip_version() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_chip_version != OT_NULL)
#define call_sys_get_chip_version() \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_chip_version()

#define ckfn_sys_drv_ioctrl() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_drv_ioctrl != OT_NULL)
#define call_sys_drv_ioctrl(omp_chn, func_id, io_args) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_drv_ioctrl(omp_chn, func_id, io_args)

#define ckfn_sys_register_sender() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_register_sender != OT_NULL)
#define call_sys_register_sender(info) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_register_sender(info)

#define ckfn_sys_unregister_sender() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_unregister_sender != OT_NULL)
#define call_sys_unregister_sender(mod_id) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_unregister_sender(mod_id)

#define ckfn_sys_register_receiver() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_register_receiver != OT_NULL)
#define call_sys_register_receiver(info) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_register_receiver(info)

#define ckfn_sys_unregister_receiver() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_unregister_receiver != OT_NULL)
#define call_sys_unregister_receiver(mod_id) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_unregister_receiver(mod_id)

#define ckfn_sys_send_data() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_send_data != OT_NULL)
#define call_sys_send_data(bind_chn, flag, data_type, pv_data) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_send_data(bind_chn, flag, data_type, pv_data)

#define ckfn_sys_reset_data() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_reset_data != OT_NULL)
#define call_sys_reset_data(mod_id, dev_id, chn_id, private) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_reset_data(mod_id, dev_id, chn_id, private)

#define ckfn_sys_get_bind_by_src() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_get_bind_by_src != OT_NULL)
#define call_sys_get_bind_by_src(src_chn, bind_dest) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_get_bind_by_src(src_chn, bind_dest)

#define ckfn_sys_get_bind_num_by_src() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_get_bind_num_by_src != OT_NULL)
#define call_sys_get_bind_num_by_src(src_chn, bind_num) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_get_bind_num_by_src(src_chn, bind_num)

#define ckfn_sys_get_bind_by_dest() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_get_bind_by_dest != OT_NULL)
#define call_sys_get_bind_by_dest(dest_chn, src_chn) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_get_bind_by_dest(dest_chn, src_chn)

#define ckfn_sys_get_smr_name() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_get_smr_name != OT_NULL)
#define call_sys_get_smr_name(src_chn, pp_smr_name) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_get_smr_name(src_chn, pp_smr_name)

#define ckfn_sys_get_full_chip_id() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_full_chip_id != OT_NULL)
#define call_sys_get_full_chip_id(chip_id) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_full_chip_id(chip_id)

#define ckfn_sys_get_chip_id() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_chip_id != OT_NULL)
#define call_sys_get_chip_id() \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_chip_id()

#define ckfn_sys_get_mipirx_ext_cfg() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_mipirx_ext_cfg != OT_NULL)
#define call_sys_get_mipirx_ext_cfg() \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_mipirx_ext_cfg()

#define ckfn_sys_get_efuse_addr() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_efuse_addr != OT_NULL)
#define call_sys_get_efuse_addr() \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_efuse_addr()
#define ckfn_sys_get_sys_base_addr() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_sys_base_addr != OT_NULL)
#define call_sys_get_sys_base_addr() \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_sys_base_addr()

#define ckfn_sys_get_time_zone() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_time_zone != OT_NULL)
#define call_sys_get_time_zone(time_zone) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_time_zone(time_zone)

#define ckfn_sys_get_gps_info() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_gps_info != OT_NULL)
#define call_sys_get_gps_info(gps_info) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_gps_info()

#define ckfn_sys_get_now_string() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_now_string != OT_NULL)
#define call_sys_get_now_string(now_time) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_now_string(now_time)

#define ckfn_sys_get_dci_coef() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_dci_coef != OT_NULL)
#define call_sys_get_dci_coef(phy_addr) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_dci_coef(phy_addr)

#define ckfn_sys_print_proc_title() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_print_proc_title != OT_NULL)
#define call_sys_print_proc_title(s, format, args...) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_print_proc_title(s, format, ##args)

#define ckfn_sys_is_tee_enable_otp() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_is_tee_enable_otp != OT_NULL)
#define call_sys_is_tee_enable_otp() \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_is_tee_enable_otp()

#ifdef CONFIG_OT_SYS_FMU_SUPPORT
#define ckfn_sys_get_fmu_page_num() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_fmu_page_num != OT_NULL)
#define call_sys_get_fmu_page_num(id, page_num) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_fmu_page_num(id, page_num)
#endif

#define ckfn_sys_get_vi_vproc_mode() \
    (func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_vi_vproc_mode != OT_NULL)
#define call_sys_get_vi_vproc_mode(mode) \
    func_entry(sys_export_func, OT_ID_SYS)->pfn_sys_get_vi_vproc_mode(mode)

#define call_sys_set_omp_chn(omp_chn, mod, dev, chn) \
    do {                                             \
        (omp_chn)->mod_id = (mod);                   \
        (omp_chn)->dev_id = (dev);                   \
        (omp_chn)->chn_id = (chn);                   \
    } while (0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif /* SYS_EXT_H */
