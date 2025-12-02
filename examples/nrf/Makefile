TARGET = pca10040
ARMGCC_PATH = $(TARGET)/armgcc

nrf:
	$(MAKE) -C $(ARMGCC_PATH) -f Makefile

flash:
	@echo Flashing: $(ARMGCC_PATH)
	$(MAKE) flash TARGET=$(TARGET)
	nrfjprog --recover
	$(MAKE) flash_softdevice -C $(ARMGCC_PATH) -f Makefile
	$(MAKE) flash -C $(ARMGCC_PATH) -f Makefile
	$(MAKE) flash-sn -C $(ARMGCC_PATH) -f Makefile
