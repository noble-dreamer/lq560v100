
.PHONY: clean motionfusion motionchip motionmng motionsensor

all: motionfusion motionchip motionmng motionsensor
	@cd src/user; $(MAKE)
	@cd src/kernel/motionsensor_chip; $(MAKE)
	@cd src/kernel/motionsensor_mng; $(MAKE)
	@cd src/kernel/motionsensor_spi; $(MAKE)
	@echo "~~~~~~~~~~~~~~Build MOTION SUCCESS~~~~~~~~~~~~~~"

motionfusion:
	@cd src/user; $(MAKE)
	@echo "~~~~~~~~~~~~~~Build MOTION_FUSION SUCCESS~~~~~~~~~~~~~~"

motionchip:
	@cd src/kernel/motionsensor_chip; $(MAKE)
	@echo "~~~~~~~~~~~~~~Build MOTION_SENSOR SUCCESS~~~~~~~~~~~~~~"

motionmng:
	@cd src/kernel/motionsensor_mng; $(MAKE)
	@echo "~~~~~~~~~~~~~~Build MOTION_MNG SUCCESS~~~~~~~~~~~~~~"

motionsensor:
	@cd src/kernel/motionsensor_spi; $(MAKE)
	@echo "~~~~~~~~~~~~~~Build MOTION_SENSOR SUCCESS~~~~~~~~~~~~~~"

clean: $(TARGET_CLEAN)
	@cd src/user; $(MAKE) clean
	@cd src/kernel/motionsensor_chip; $(MAKE) clean
	@cd src/kernel/motionsensor_mng; $(MAKE) clean
	@cd src/kernel/motionsensor_spi; $(MAKE) clean
	@echo "~~~~~~~~~~~~~~Build MOTION_CLEAN SUCCESS~~~~~~~~~~~~~~"

