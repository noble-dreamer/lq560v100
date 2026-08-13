CUR_DIR = $(shell pwd)

MAIN_DIR = $(CUR_DIR)/../../../../../..
ifeq ($(MAIN_CFG), )
    MAIN_CFG := $(MAIN_DIR)/build/main_cfg.mak
    include $(MAIN_CFG)
endif
SMR_DIR = $(CUR_DIR)/../../../../src/component/smr/src/include/
ifeq ($(CONFIG_OT_MOTIONFUSION_SUPPORT), y)
# Sensor Type: ICM20690 / ICM42670
SENSOR_TYPE ?= ICM42670

# Transfer Type: I2C SPI
export TRANSFER_TYPE := SPI

# ICM proc info function : y or n
ifeq ($(SENSOR_TYPE), ICM20690)
    export ICM20690_PROC = y
else ifeq ($(SENSOR_TYPE), ICM42670)
    export ICM42670_PROC = y
endif

SRCS = ./main/motionsensor.c

EXTRA_CFLAGS += -I$(PWD)/include
EXTRA_CFLAGS += -I$(PWD)/main
EXTRA_CFLAGS += -I$(PWD)/../include/
EXTRA_CFLAGS += -I$(PWD)/../ext_inc
EXTRA_CFLAGS += -I$(PWD)/../motionsensor_spi
EXTRA_CFLAGS += -I$(PWD)/../motionsensor_mng/include
EXTRA_CFLAGS += -I$(PWD)/../../../include
EXTRA_CFLAGS += -I$(PWD)/sensor_dev/icm42670

ifeq ($(CONFIG_OT_PROC_SHOW_SUPPORT),y)
SRCS += ./proc/motionsensor_chip_proc.c
EXTRA_CFLAGS += -I$(PWD)/proc
ifeq ($(ICM20690_PROC),y)
	ccflags-y += -DIMU_ICM20690
else ifeq ($(ICM42670_PROC),y)
    ccflags-y += -DIMU_ICM42670
endif
endif

ifeq ($(SENSOR_TYPE), ICM20690)
    SRCS += ./sensor_dev/icm20690/icm20690.c
	SRCS += ./sensor_dev/icm20690/motionsensor_gpio.c
    EXTRA_CFLAGS += -I$(PWD)/sensor_dev/icm20690
else
    SRCS += ./sensor_dev/icm42670/icm42670.c
    SRCS += ./sensor_dev/icm42670/motionsensor_gpio.c
    EXTRA_CFLAGS += -I$(PWD)/sensor_dev/icm42670
endif

ifeq ($(TRANSFER_TYPE), I2C)
    SRCS += ./transfer_type/i2c/i2c_dev.c
    ccflags-y += -DTRANSFER_I2C
    EXTRA_CFLAGS += -I$(PWD)/transfer_type/i2c
    EXTRA_CFLAGS += -I$(TRANSFER_DIR)/i2c
endif

ifeq ($(TRANSFER_TYPE), SPI)
	ccflags-y += -DTRANSFER_SPI
    EXTRA_CFLAGS += -I$(PWD)/transfer_type/spi
    EXTRA_CFLAGS += -I$(TRANSFER_DIR)/spi
endif
EXTRA_CFLAGS += -I${SMR_DIR}

SRCS_O := $(SRCS)
TARGET := motionsensor_chip
DEPEND_SYMBOL := $(MAIN_DIR)/src/kal/src/linux/kernel/obj/Module.symvers
DEPEND_SYMBOL += $(MAIN_DIR)/src/component/smr/src/kernel/obj/Module.symvers
include $(MAKE_DRV_FILE)

#CONFIG_OT_MOTIONFUSION_SUPPORT endif
else
all:
	@echo "motionsensor module not compile"
clean:
	@echo "motionsensor module don't need clean"
endif
