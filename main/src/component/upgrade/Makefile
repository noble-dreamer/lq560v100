CUR_DIR = $(shell pwd)

ifeq ($(MAIN_CFG), )
    MAIN_DIR = $(CUR_DIR)/../../..
    MAIN_CFG := $(MAIN_DIR)/build/main_cfg.mak
    include $(MAIN_CFG)
endif

# Build ota_prepare and the FunctionFS USB receiver. After a package is
# received, the receiver execs /usr/bin/ota_prepare, which is installed
# from the ota_prepare directory below (component/upgrade/ota_prepare).
TARGET = src sample ota_prepare usb_receiver
TARGET_CLEAN = $(addsuffix _clean,$(TARGET))

.PHONY: $(TARGET) $(TARGET_CLEAN)

all: $(TARGET)
	@echo "~~~~~~~~~~~~~~Build All Component SUCCESS~~~~~~~~~~~~~~"

clean: $(TARGET_CLEAN)
	@echo "~~~~~~~~~~~~~~Clean Component SUCCESS~~~~~~~~~~~~~~"

sample: src
	@echo "~~~~~~~~~~Start build $@~~~~~~~~~~"
	@cd $@ && $(MAKE)

ota_prepare: src
	@echo "~~~~~~~~~~Start build $@~~~~~~~~~~"
	@cd $@ && $(MAKE)

usb_receiver:
	@echo "~~~~~~~~~~Start build $@~~~~~~~~~~"
	@cd $@ && $(MAKE)

src:
	@echo "~~~~~~~~~~Start build $@~~~~~~~~~~"
	@cd $@ && $(MAKE)

$(TARGET_CLEAN):
	@echo "~~~~~~~~~~Start clean $(subst _clean,,$@)~~~~~~~~~~"
	@cd $(subst _clean,,$@) && $(MAKE) clean
