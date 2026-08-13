CUR_DIR=$(shell pwd)
MAIN_DIR = $(CUR_DIR)/../../../../../
include $(CUR_DIR)/../sample_cfg.mak

# 定义对象文件目录
OBJ_DIR := obj

# 定义所有目标（三个可执行文件）
TARGETS := sample_dis dis_send dis_dump

# 公共源文件（无main函数）
COMMON_SRCS := tools/ini/src/ini.c
COMMON_OBJS := $(addprefix $(OBJ_DIR)/, $(notdir $(COMMON_SRCS:%.c=%.o)))

# sample_dis 需要的源文件
SAMPLE_DIS_SRCS := sample/sample_dis.c sample/sample_gyro_dis.c
SAMPLE_DIS_OBJS := $(addprefix $(OBJ_DIR)/, $(notdir $(SAMPLE_DIS_SRCS:%.c=%.o)))

# dis_send 需要的源文件
DIS_SEND_SRCS := tools/send/dis_send.c
DIS_SEND_OBJS := $(addprefix $(OBJ_DIR)/, $(notdir $(DIS_SEND_SRCS:%.c=%.o)))

# dis_dump 需要的源文件
DIS_DUMP_SRCS := tools/dump/dis_dump.c
DIS_DUMP_OBJS := $(addprefix $(OBJ_DIR)/, $(notdir $(DIS_DUMP_SRCS:%.c=%.o)))

# 所有对象文件列表
ALL_OBJS := $(COMMON_OBJS) $(SAMPLE_DIS_OBJS) $(DIS_SEND_OBJS) $(DIS_DUMP_OBJS)

# 头文件路径
CFLAGS += $(COMM_INC)
CFLAGS += -I$(CUR_DIR)/tools/ini/include
CFLAGS += -I$(CUR_DIR)/include
CFLAGS += -I$(CUR_DIR)/../vo

# 依赖库配置
DEP_LIBS += $(SYS_LIBS)
DEP_LIBS += $(VI_LIBS)
DEP_LIBS += $(VPROC_LIBS)
DEP_LIBS += $(REL_LIB)/libot_vo.a
DEP_LIBS += $(REL_LIB)/libot_venc.a
DEP_LIBS += $(LIBS_LD_CFLAGS)
DEP_LIBS += $(SAMPLE_COMMON_LIB)
DEP_LIBS += $(MIPI_RX_LIBS)
DEP_LIBS += $(MIPI_TX_LIBS)
DEP_LIBS += $(OGE_LIBS)
DEP_LIBS += $(REL_LIB)/libot_motionfusion_adapt.a
DEP_LIBS += $(REL_LIB)/libot_motionfusion.a

SAMPLE_COMMON_DIR:= $(PWD)/../common/

.PHONY : clean all

all: $(OBJ_DIR) $(TARGETS)

# 创建对象目录
$(OBJ_DIR):
	@mkdir -p $@

# 编译公共文件
$(OBJ_DIR)/ini.o: tools/ini/src/ini.c
	$(CC) $(CFLAGS) -c $< -o $@

# 编译sample_dis相关文件
$(OBJ_DIR)/sample_dis.o: sample/sample_dis.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/sample_gyro_dis.o: sample/sample_gyro_dis.c
	$(CC) $(CFLAGS) -c $< -o $@

# 编译dis_send相关文件
$(OBJ_DIR)/dis_send.o: tools/send/dis_send.c
	$(CC) $(CFLAGS) -c $< -o $@

# 编译dis_dump相关文件
$(OBJ_DIR)/dis_dump.o: tools/dump/dis_dump.c
	$(CC) $(CFLAGS) -c $< -o $@

# sample_dis 可执行文件
sample_dis: $(COMMON_OBJS) $(SAMPLE_DIS_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ \
		-lpthread -lm \
		-Wl,--start-group \
		$(DEP_LIBS) \
		$(SENSOR_LIBS) \
		$(AUDIO_LIBA) \
		$(INIPARSER_LIB) \
		$(REL_LIB)/libsecurec.a \
		-Wl,--end-group

# dis_send 可执行文件
dis_send: $(COMMON_OBJS) $(DIS_SEND_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ \
		-lpthread -lm \
		-Wl,--start-group \
		$(DEP_LIBS) \
		$(SENSOR_LIBS) \
		$(AUDIO_LIBA) \
		$(INIPARSER_LIB) \
		$(REL_LIB)/libsecurec.a \
		-Wl,--end-group

# dis_dump 可执行文件
dis_dump: $(COMMON_OBJS) $(DIS_DUMP_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ \
		-lpthread -lm \
		-Wl,--start-group \
		$(DEP_LIBS) \
		$(SENSOR_LIBS) \
		$(AUDIO_LIBA) \
		$(INIPARSER_LIB) \
		$(REL_LIB)/libsecurec.a \
		-Wl,--end-group

clean:
	@rm -f $(TARGETS)
	@rm -rf $(OBJ_DIR)
	@rm -f $(COMM_OBJ)

cleanstream:
	@rm -f *.h264
	@rm -f *.h265