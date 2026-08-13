CUR_DIR=$(shell pwd)
MAIN_DIR = $(CUR_DIR)/../../../../../
include $(CUR_DIR)/../sample_cfg.mak

CONFIG_SCENE_AUTO := y

SRC_PATH := $(shell pwd)

CUR_SRCS := $(SRC_PATH)/uvc_media.c $(SRC_PATH)/uvc_dpu.c $(SRC_PATH)/stereo_vo.c 

ifeq ($(CONFIG_SCENE_AUTO), y)

CFLAGS += -DSCENE_AUTO_COMPILE

SCENE_PATH := $(SRC_PATH)/../scene_auto

INC_FLAGS += -I $(SCENE_PATH)/include -I $(SCENE_PATH)/src/core -I $(SCENE_PATH)/tools/configaccess/include

CUR_SRCS += $(SCENE_PATH)/src/core/ot_scene_loadparam.c
CUR_SRCS += $(SCENE_PATH)/src/core/ot_scene_setparam.c
CUR_SRCS += $(SCENE_PATH)/src/core/ot_scene.c
CUR_SRCS += $(SCENE_PATH)/src/core/scene_setparam_inner.c
CUR_SRCS += $(SCENE_PATH)/tools/configaccess/src/ini.c
CUR_SRCS += $(SCENE_PATH)/tools/configaccess/src/ot_confaccess.c
CUR_SRCS += $(SRC_PATH)/uvc_scene_auto.c
endif

TARGET := uvc_test

TARGET_PATH := $(PWD)

OBJS  := $(CUR_SRCS:%.c=%.o)

CFLAGS += $(INC_FLAGS)
CFLAGS += -fPIC -no-pie
CFLAGS += $(COMM_INC)
CFLAGS += -I$(CUR_DIR)

SYS_LIBS += $(REL_LIB)/libprotobuf-c.a
SYS_LIBS += $(REL_LIB)/libot_smr.a
SYS_LIBS += $(REL_LIB)/libot_km.a

DEP_LIBS += $(SYS_LIBS)
ifeq ($(CONFIG_SCENE_AUTO), y)
NPU_LIBS := $(REL_LIB)/libot_avp_npu_impl.a
NPU_LIBS += $(REL_LIB)/libot_avp_npu_intf.a
NPU_LIBS += $(REL_LIB)/libot_avp_npu.a
DEP_LIBS += $(NPU_LIBS)
endif

DPU_LIBS += $(REL_LIB)/libot_avp_dpu_impl.a
DPU_LIBS += $(REL_LIB)/libot_avp_dpu_rect.a
DPU_LIBS += $(REL_LIB)/libot_avp_dpu_match.a

DEP_LIBS += ${DPU_LIBS}
DEP_LIBS += $(VI_LIBS)
DEP_LIBS += $(VPROC_LIBS)
DEP_LIBS += $(VENC_LIBS)
DEP_LIBS += $(MIPI_RX_LIBS)
DEP_LIBS += $(MIPI_TX_LIBS)
DEP_LIBS += $(OGE_LIBS)
DEP_LIBS += $(LIBS_LD_CFLAGS)
DEP_LIBS += $(SAMPLE_COMMON_LIB)

ifeq ($(CONFIG_SCENE_AUTO), y)
DEP_LIBS += $(REL_LIB)/libot_proc.a
DEP_LIBS += $(REL_LIB)/libot_mce_det.a
DEP_LIBS += $(REL_LIB)/libot_haze_level.a
DEP_LIBS += $(REL_LIB)/libot_abnr.a
DEP_LIBS += $(REL_LIB)/libot_iqt.a
DEP_LIBS += $(REL_LIB)/libot_deflicker.a
DEP_LIBS += $(REL_LIB)/libot_scene_auto.a
endif
DEP_LIBS += $(REL_LIB)/libot_vo.a

.PHONY : clean all

all: $(TARGET)

$(TARGET):$(COMM_OBJ) $(OBJS)
	$(CC) $(CFLAGS) -lpthread -lm -o $(TARGET_PATH)/$@ $^ -Wl,--start-group $(DEP_LIBS) $(SENSOR_LIBS) $(AUDIO_LIBA)  $(INIPARSER_LIB)  $(REL_LIB)/libsecurec.a -Wl,--end-group


clean:
	@rm -f $(TARGET_PATH)/$(TARGET)
	@rm -f $(OBJS)
	@rm -f $(COMM_OBJ)