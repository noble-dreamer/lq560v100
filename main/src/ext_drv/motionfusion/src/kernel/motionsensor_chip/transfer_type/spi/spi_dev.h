/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#ifndef SPI_DEV_H
#define SPI_DEV_H

#include "ot_eis_type.h"
#include "ot_osal.h"
#include "ot_debug.h"
#include <linux/spi/spi.h>

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

ot_s32 motionsersor_spi_write(struct spi_device *ot_spi, ot_u8 addr, const ot_u8 *data, ot_u32 cnt);
ot_s32 motionsersor_spi_read(struct spi_device *ot_spi, ot_u8 addr, ot_u8 *data, ot_u32 cnt);
ot_s32 motionsersor_spi_init(struct spi_device **ot_spi);
ot_s32 motionsersor_spi_deinit(const struct spi_device *spi_device);

#define msensor_err_trace(fmt, ...) \
    OT_ERR_TRACE(OT_ID_MOTIONSENSOR, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif
