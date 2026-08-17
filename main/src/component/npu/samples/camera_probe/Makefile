CUR_DIR=$(shell pwd)
MAIN_DIR = $(CUR_DIR)/../../../../../
COMMON_DIR := $(MAIN_DIR)/src/component/media/sample/common
include $(MAIN_DIR)/src/component/media/sample/sample_cfg.mak

SCENE_PATH := $(MAIN_DIR)/src/component/media/sample/scene_auto

CUR_SRCS := $(wildcard $(CUR_DIR)/*.c)
CUR_SRCS += $(SCENE_PATH)/src/core/ot_scene_loadparam.c
CUR_SRCS += $(SCENE_PATH)/src/core/ot_scene_setparam.c
CUR_SRCS += $(SCENE_PATH)/src/core/ot_scene.c
CUR_SRCS += $(SCENE_PATH)/src/core/scene_setparam_inner.c
CUR_SRCS += $(SCENE_PATH)/tools/configaccess/src/ini.c
CUR_SRCS += $(SCENE_PATH)/tools/configaccess/src/ot_confaccess.c

TARGET := camera_probe
OBJ_DIR := $(CUR_DIR)/obj
OBJS := $(addprefix $(OBJ_DIR)/, $(notdir $(CUR_SRCS:.c=.o)))
VPATH := $(sort $(dir $(CUR_SRCS)))

INC_FLAGS += -I $(SCENE_PATH)/include
INC_FLAGS += -I $(SCENE_PATH)/src/core
INC_FLAGS += -I $(SCENE_PATH)/tools/configaccess/include

CFLAGS += $(INC_FLAGS)
CFLAGS += $(COMM_INC)
CFLAGS += -I$(CUR_DIR)
CFLAGS += -fPIC -no-pie

SYS_LIBS := $(REL_LIB)/libprotobuf-c.a
SYS_LIBS += $(REL_LIB)/libot_media_common.a
SYS_LIBS += $(REL_LIB)/libot_media_sys.a
SYS_LIBS += $(REL_LIB)/libot_media_bind.a
SYS_LIBS += $(REL_LIB)/libot_smr.a
SYS_LIBS += $(REL_LIB)/libot_buffer_pool.a
SYS_LIBS += $(REL_LIB)/libot_vrb.a
SYS_LIBS += $(REL_LIB)/libot_pipline.a
SYS_LIBS += $(REL_LIB)/libot_utils.a
SYS_LIBS += $(REL_LIB)/libcjson.a
SYS_LIBS += $(REL_LIB)/libot_otp.a
SYS_LIBS += $(REL_LIB)/libot_board_info.a

SCENE_LIBS := $(REL_LIB)/libot_scene_auto.a
SCENE_LIBS += $(REL_LIB)/libot_proc.a
SCENE_LIBS += $(REL_LIB)/libot_mce_det.a
SCENE_LIBS += $(REL_LIB)/libot_haze_level.a
SCENE_LIBS += $(REL_LIB)/libot_abnr.a
SCENE_LIBS += $(REL_LIB)/libot_iqt.a
SCENE_LIBS += $(REL_LIB)/libot_deflicker.a

NPU_LIBS := $(REL_LIB)/libot_avp_npu_impl.a
NPU_LIBS += $(REL_LIB)/libot_avp_npu_intf.a
NPU_LIBS += $(REL_LIB)/libot_avp_npu.a

DPU_LIBS := $(REL_LIB)/libot_avp_dpu_impl.a
DPU_LIBS += $(REL_LIB)/libot_avp_dpu_rect.a
DPU_LIBS += $(REL_LIB)/libot_avp_dpu_match.a

DEP_LIBS += $(SYS_LIBS)
DEP_LIBS += $(NPU_LIBS)
DEP_LIBS += $(DPU_LIBS)
DEP_LIBS += $(SCENE_LIBS)
DEP_LIBS += $(VI_LIBS)
DEP_LIBS += $(VPROC_LIBS)
DEP_LIBS += $(MIPI_RX_LIBS)
DEP_LIBS += $(MIPI_TX_LIBS)
DEP_LIBS += $(REL_LIB)/libot_vo.a
DEP_LIBS += $(OGE_LIBS)
DEP_LIBS += $(LIBS_LD_CFLAGS)
DEP_LIBS += $(SAMPLE_COMMON_LIB)

.PHONY: clean all

all: $(TARGET)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -lpthread -lm -o $@ $^ -Wl,--start-group $(DEP_LIBS) $(REL_LIB)/libsecurec.a -Wl,--end-group

clean:
	@rm -f $(TARGET)
	@rm -rf $(OBJ_DIR)
