TARGET = pca10040
ARMGCC_PATH = $(TARGET)/armgcc

CIOT_PATH := $(subst \,/,$(CIOT_PATH))
CIOT_DIR = $(CIOT_PATH)/ciot_c
PROJ_DIR = ./

nrf:
	$(MAKE) -C $(ARMGCC_PATH) -f Makefile

flash:
	@echo Flashing: $(ARMGCC_PATH)
	$(MAKE) flash TARGET=$(TARGET)
	nrfjprog --recover
	$(MAKE) flash_softdevice -C $(ARMGCC_PATH) -f Makefile
	$(MAKE) flash -C $(ARMGCC_PATH) -f Makefile
	$(MAKE) flash-sn -C $(ARMGCC_PATH) -f Makefile

test:
	$(MAKE) -C $(ARMGCC_PATH) -f Makefile test