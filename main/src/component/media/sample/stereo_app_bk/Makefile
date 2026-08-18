CUR_DIR=$(shell pwd)
MAIN_DIR = $(CUR_DIR)/../../../../../
include $(CUR_DIR)/../sample_cfg.mak

# Override sensor type for stereo cameras (SC132GS dual-camera)
SENSOR0_TYPE = SMS_SC132GS_MIPI_1_5M_30FPS_RAW12
SENSOR1_TYPE = SMS_SC132GS_MIPI_1_5M_30FPS_RAW12_DIRECT_SLAVE

CONFIG_SCENE_AUTO := y

SRC_PATH := $(shell pwd)

CUR_SRCS := $(SRC_PATH)/stereo_main.c
CUR_SRCS += $(SRC_PATH)/stereo_camera.c
CUR_SRCS += $(SRC_PATH)/stereo_media.c
CUR_SRCS += $(SRC_PATH)/stereo_queue.c
CUR_SRCS += $(SRC_PATH)/stereo_cve.c
CUR_SRCS += $(shell if [ -f $(SRC_PATH)/../media_ai/ot_avp_cve_alg_lib.c ]; then echo $(SRC_PATH)/../media_ai/ot_avp_cve_alg_lib.c; fi)
CUR_SRCS += $(SRC_PATH)/stereo_npu.c
CUR_SRCS += $(SRC_PATH)/stereo_subpixel.c
CUR_SRCS += $(SRC_PATH)/stereo_venc.c
CUR_SRCS += $(SRC_PATH)/stereo_lz4.c
CUR_SRCS += $(SRC_PATH)/stereo_depth_lut.c
CUR_SRCS += $(SRC_PATH)/stereo_rectify.c
CUR_SRCS += $(SRC_PATH)/stereo_network.c
CUR_SRCS += $(SRC_PATH)/stereo_raw_capture.c
CUR_SRCS += $(SRC_PATH)/stereo_sec.c

ifeq ($(CONFIG_SCENE_AUTO), y)
SCENE_PATH := $(SRC_PATH)/../scene_auto
INC_FLAGS += -I $(SCENE_PATH)/include -I $(SCENE_PATH)/src/core -I $(SCENE_PATH)/tools/configaccess/include
CUR_SRCS += $(SCENE_PATH)/src/core/ot_scene_loadparam.c
CUR_SRCS += $(SCENE_PATH)/src/core/ot_scene_setparam.c
CUR_SRCS += $(SCENE_PATH)/src/core/ot_scene.c
CUR_SRCS += $(SCENE_PATH)/src/core/scene_setparam_inner.c
CUR_SRCS += $(SCENE_PATH)/tools/configaccess/src/ini.c
CUR_SRCS += $(SCENE_PATH)/tools/configaccess/src/ot_confaccess.c
CUR_SRCS += $(SRC_PATH)/stereo_scene_auto.c
endif

TARGET := stereo_app
TARGET_PATH := $(CUR_DIR)
OBJS := $(CUR_SRCS:%.c=%.o)

CFLAGS += $(INC_FLAGS)
CFLAGS += -fPIC -no-pie
CFLAGS += $(COMM_INC)
CFLAGS += -I$(CUR_DIR)
CFLAGS += -I$(CUR_DIR)/../media_ai
CFLAGS += -I$(MAIN_DIR)/src/component/smr/src/include
CFLAGS += -I$(MAIN_DIR)/src/component/security/include

# Performance timing: set to 'y' to enable per-frame timing logs
CONFIG_PERF_TIMING := y
ifeq ($(CONFIG_PERF_TIMING), y)
CFLAGS += -DSTEREO_PERF_TIMING
endif

SYS_LIBS += $(REL_LIB)/libprotobuf-c.a
SYS_LIBS += $(REL_LIB)/libot_smr.a
SYS_LIBS += $(REL_LIB)/libot_km.a
SYS_LIBS += $(REL_LIB)/libot_cipher.a

NPU_LIBS := $(REL_LIB)/libot_avp_npu_impl.a
NPU_LIBS += $(REL_LIB)/libot_avp_npu_intf.a
NPU_LIBS += $(REL_LIB)/libot_avp_npu.a

CVE_LIBS := $(REL_LIB)/libot_avp_cve.a
CVE_LIBS += $(REL_LIB)/libot_avp_cve_intf.a
CVE_LIBS += $(REL_LIB)/libot_avp_cve_impl.a

DEP_LIBS += $(SYS_LIBS)
DEP_LIBS += $(NPU_LIBS)
DEP_LIBS += $(CVE_LIBS)
DEP_LIBS += $(VI_LIBS)
DEP_LIBS += $(VPROC_LIBS)
DEP_LIBS += $(VENC_LIBS)
DEP_LIBS += $(MIPI_RX_LIBS)
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

# VO + VO_intf + OGE/GFB + MIPI_TX libs needed by sample_common.a transitive dependencies
DEP_LIBS += $(REL_LIB)/libot_vo.a
DEP_LIBS += $(REL_LIB)/libot_vo_intf.a
DEP_LIBS += $(OGE_LIBS)
DEP_LIBS += $(MIPI_TX_LIBS)

.PHONY: clean all auth_gen

all: install

install: $(TARGET)
	@mkdir -p $(MAIN_DIR)/src/platform/fs/userfs/base/stereo
	@cp $(TARGET_PATH)/$(TARGET) $(MAIN_DIR)/src/platform/fs/userfs/base/stereo

$(TARGET): $(COMM_OBJ) $(OBJS)
	$(CC) $(CFLAGS) -lpthread -lm -o $(TARGET_PATH)/$@ $^ -Wl,--start-group $(DEP_LIBS) $(SENSOR_LIBS) $(AUDIO_LIBA) $(INIPARSER_LIB) $(REL_LIB)/libsecurec.a -Wl,--end-group

# ---------------------------------------------------------------------------
# auth_gen: standalone board-side authorization generator (separate binary)
# Cross-compiled with SDK toolchain, deployed to /tmp/auth_gen by gen_license.sh
# ---------------------------------------------------------------------------
AUTH_GEN := auth_gen
AUTH_GEN_SRC := $(SRC_PATH)/auth_gen.c
AUTH_GEN_OBJ := $(AUTH_GEN_SRC:%.c=%.o)
AUTH_GEN_LIBS := $(REL_LIB)/libot_cipher.a
AUTH_GEN_LIBS += $(REL_LIB)/libot_km.a
AUTH_GEN_LIBS += $(REL_LIB)/libot_media_common.a
AUTH_GEN_LIBS += $(REL_LIB)/libot_media_sys.a
AUTH_GEN_LIBS += $(REL_LIB)/libot_smr.a
AUTH_GEN_LIBS += $(REL_LIB)/libot_otp.a
AUTH_GEN_LIBS += $(REL_LIB)/libsecurec.a

$(AUTH_GEN): $(AUTH_GEN_OBJ)
	$(CC) $(CFLAGS) -lpthread -lm -o $(TARGET_PATH)/$@ $^ -Wl,--start-group $(AUTH_GEN_LIBS) -Wl,--end-group

clean:
	@rm -f $(TARGET_PATH)/$(TARGET)
	@rm -f $(TARGET_PATH)/$(AUTH_GEN)
	@rm -f $(OBJS)
	@rm -f $(AUTH_GEN_OBJ)
	@rm -f $(COMM_OBJ)
