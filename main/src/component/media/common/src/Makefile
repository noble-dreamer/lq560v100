CUR_DIR = $(shell pwd)
MAIN_DIR = $(CUR_DIR)/../../../../../
ifeq ($(MAIN_CFG), )
    MAIN_CFG := $(MAIN_DIR)/build/main_cfg.mak
    include $(MAIN_CFG)
endif

# user have to impl TARGET SRC_DIR LIB_DIR OBJ_DIR INSTLL_HEADERS CUR_INC
TARGET=ot_utils

SRC_DIR=$(CUR_DIR)
LIB_DIR=$(CUR_DIR)/../libs
OBJ_DIR = $(CUR_DIR)/obj


CUR_INC := -I$(CUR_DIR)/../include
CUR_INC += -I$(CUR_DIR)/include
#CUR_INC += -I$(CUR_DIR)/kernel
#CUR_INC += -I$(CUR_DIR)/user/res_mng
CUR_INC += -I$(SDK_COMMON_DIR)
CUR_INC += -I$(SDK_COMMON_DIR)/include
CUR_INC += -I$(MEDIA_COMMON_DIR)
CUR_INC += -I$(MAIN_COMMON_DIR)
#CUR_INC += -I$(MAIN_COMMON_DIR)/../securec/include
#CUR_INC += -I$(CUR_DIR)/../../buf_pool/include


# user have to impl CFLAGS LDFLAGS
CFLAGS += $(LIBS_CFLAGS)
LDFLAGS += $(LIBS_LD_CFLAGS)


include $(MAIN_DIR)/build/lib_template.mak

