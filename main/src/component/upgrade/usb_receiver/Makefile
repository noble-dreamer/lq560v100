CUR_DIR=$(shell pwd)

ifeq ($(MAIN_CFG), )
    MAIN_DIR = $(CUR_DIR)/../../../..
    MAIN_CFG := $(MAIN_DIR)/build/main_cfg.mak
    include $(MAIN_CFG)
endif

TARGET := usb_receiver
TARGET_PATH := $(CUR_DIR)
CUR_SRCS := $(wildcard *.c)

CFLAGS += -I$(CUR_DIR)/
CFLAGS += -I$(CUR_DIR)/../include $(COMMON_INCLUDE)

CFLAGS += -I$(REL_INC)

LD_FLAGS += $(LIBS_LD_CFLAGS)

# target source
CUR_OBJS  := $(CUR_SRCS:%.c=%.o)

CUR_LDFLAGS += $(COMM_INC)

.PHONY : clean all

all: $(TARGET)

$(TARGET): $(CUR_OBJS)
	@$(CC) $(LD_FLAGS) -o $(TARGET_PATH)/$@ $^ $(LD_FLAGS)

clean:
	@rm -f $(TARGET_PATH)/$(TARGET)
	@rm -f $(CUR_OBJS)
