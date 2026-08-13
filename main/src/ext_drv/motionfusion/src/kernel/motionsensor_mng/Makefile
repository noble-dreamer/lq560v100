CUR_DIR = $(shell pwd)

MAIN_DIR = $(CUR_DIR)/../../../../../..
ifeq ($(MAIN_CFG), )
    MAIN_CFG := $(MAIN_DIR)/build/main_cfg.mak
    include $(MAIN_CFG)
endif
SMR_DIR = $(CUR_DIR)/../../../../src/component/smr/src/include/

ifeq ($(CONFIG_OT_MOTIONFUSION_SUPPORT), y)

SRCS += ./src/motionsensor_exe.c
SRCS += ./src/motionsensor_buf.c
ifeq ($(CONFIG_OT_PROC_SHOW_SUPPORT), y)
SRCS += ./src/motionsensor_mng_proc.c
endif

EXTRA_CFLAGS := -I$(PWD)/include/
EXTRA_CFLAGS += -I$(PWD)/../ext_inc/
EXTRA_CFLAGS += -I$(PWD)/../../../include
EXTRA_CFLAGS += -I${SMR_DIR}

SRCS_O := $(SRCS)
TARGET   := motionsensor_mng
DEPEND_SYMBOL := $(MAIN_DIR)/src/kal//src/linux/kernel/obj/Module.symvers
DEPEND_SYMBOL += $(MAIN_DIR)/src/component/smr/src/kernel/obj/Module.symvers
include $(MAKE_DRV_FILE)

#CONFIG_OT_MOTIONFUSION_SUPPORT endif
else
all:
	@echo "motionsensor module not compile"
clean:
	@echo "motionsensor module don't need clean"
endif

