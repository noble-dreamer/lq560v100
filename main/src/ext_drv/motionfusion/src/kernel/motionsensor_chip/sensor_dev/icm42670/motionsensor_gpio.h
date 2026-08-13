/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */


#ifndef MOTIONSENSOR_GPIO_H
#define MOTIONSENSOR_GPIO_H

#include "ot_type.h"
#include<linux/gpio.h>

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#define gpio_num(gpio_chip_num, gpio_offset_num)  ((gpio_chip_num) * 8 + (gpio_offset_num))

ot_s32 gpio_init(ot_void);
ot_s32 gpio_deinit(ot_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif
