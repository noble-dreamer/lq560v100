CUR_DIR = $(shell pwd)

MAIN_DIR = $(CUR_DIR)/../../../../../..
ifeq ($(MAIN_CFG), )
    MAIN_CFG := $(MAIN_DIR)/build/main_cfg.mak
    include $(MAIN_CFG)
endif

ifeq ($(CONFIG_OT_MOTIONFUSION_SUPPORT), y)

SRCS := motionsensor_spi.c

SRCS_O := $(SRCS)
TARGET   := ot_spi
DEPEND_SYMBOL := $(MAIN_DIR)/src/kal//src/linux/kernel/obj/Module.symvers

include $(MAKE_DRV_FILE)

#CONFIG_OT_MOTIONFUSION_SUPPORT endif
else
all:
	@echo "motionsensor module not compile"
clean:
	@echo "motionsensor module don't need clean"
endif