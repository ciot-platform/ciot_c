# PROJ_DIR = ../../..
# HDR_DIR = $(PROJ_DIR)/..
# HG_TCP_DIR = $(HDR_DIR)/hg_tcp

CIOT_PATH := $(subst \,/,$(CIOT_PATH))
NRF_PATH := $(subst \,/,$(NRF_PATH))

NRF_SDK_12 = $(NRF_PATH)/nRF5_SDK_12
NRF_SDK_17 = $(NRF_PATH)/nRF5_SDK_17
CIOT_DIR = $(CIOT_PATH)/ciot_c