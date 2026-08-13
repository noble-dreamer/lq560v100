/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */


#ifndef MOTIONSENSOR_SPI_H
#define MOTIONSENSOR_SPI_H

#include "ot_type.h"

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#define SSP_READ_ALT  0x1
#define SSP_WRITE_ALT 0X3

typedef struct {
    ot_u32 spi_no;
    ot_u8 dev_addr;
    ot_u32 dev_byte_num;
    ot_u32 reg_addr;
    ot_u32 addr_byte_num;
    ot_u32 data;
    ot_u32 data_byte_num;
} spi_data;

ot_s32 ot_motionsensor_ssp_get_num(ot_void);
ot_s32 ot_motionsensor_ssp_write_alt(ot_u32 ssp_no, ot_u8 reg_addr, const ot_u8 *data);
ot_u16 ot_motionsensor_ssp_read_alt(ot_u32 ssp_no, ot_u8 reg_addr, ot_u8 *reg_data,
    ot_u32 cnt, ot_bool fifo_mode);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif
