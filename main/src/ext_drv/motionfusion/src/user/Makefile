CUR_DIR_O = $(shell pwd)

MAIN_DIR = $(CUR_DIR_O)/../../../../..
ifeq ($(MAIN_CFG), )
    MAIN_CFG := $(MAIN_DIR)/build/main_cfg.mak
    include $(MAIN_CFG)
endif
CUR_DIR=$(CUR_DIR_O)/../../

# user have to impl TARGET SRC_DIR LIB_DIR OBJ_DIR INSTLL_HEADERS CUR_INC
TARGET=ot_motionfusion_adapt

SRC_DIR=$(CUR_DIR)/src/user
LIB_DIR=$(CUR_DIR)/libs
OBJ_DIR = $(CUR_DIR)/obj

INSTALL_HEADERS = $(wildcard $(CUR_DIR)/include/*.h)
INSTALL_KOS = $(wildcard $(CUR_DIR)/ko/*.ko)
INSTALL_LIBS = $(wildcard $(CUR_DIR)/libs/libot_motionfusion.*)

CUR_INC := -I$(CUR_DIR)/include
CUR_INC += -I$(CUR_DIR)/src/include

CUR_INC += -I$(SDK_COMMON_DIR)
CUR_INC += -I$(MEDIA_COMMON_DIR)
CUR_INC += -I$(MAIN_COMMON_DIR)

# user have to impl CFLAGS LDFLAGS
CFLAGS += $(LIBS_CFLAGS)
LDFLAGS += $(LIBS_LD_CFLAGS)


include $(MAIN_DIR)/build/lib_template.mak

