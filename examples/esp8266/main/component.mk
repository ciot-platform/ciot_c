#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

UNAME_S := $(shell uname -s)

MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
  CIOT_PATH := $(shell realpath --relative-to=$(MAKEFILE_DIR) $(CIOT_PATH))
else ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
  CIOT_PATH := $(shell realpath --relative-to=$(MAKEFILE_DIR) $(shell cygpath -u "$$CIOT_PATH"))
else ifeq ($(findstring MSYS,$(UNAME_S)),MSYS)
  CIOT_PATH := $(shell realpath --relative-to=$(MAKEFILE_DIR) $(shell cygpath -u "$$CIOT_PATH"))
else
  CIOT_PATH := $(shell realpath --relative-to=$(MAKEFILE_DIR) $(CIOT_PATH))
endif

EXTRA_COMPONENT_DIRS += CIOT_PATH/ciot_c

COMPONENT_ADD_INCLUDEDIRS += $(CIOT_PATH)/ciot_c/include
COMPONENT_ADD_INCLUDEDIRS += $(CIOT_PATH)/ciot_c/src
COMPONENT_ADD_INCLUDEDIRS += $(CIOT_PATH)/ciot_c/src/protos
COMPONENT_ADD_INCLUDEDIRS += $(CIOT_PATH)/ciot_c/src/nanopb
COMPONENT_ADD_INCLUDEDIRS += $(CIOT_PATH)/ciot_c/src/nanomodbus

COMPONENT_SRCDIRS += $(CIOT_PATH)/ciot_c/src/core
COMPONENT_SRCDIRS += $(CIOT_PATH)/ciot_c/src/common
COMPONENT_SRCDIRS += $(CIOT_PATH)/ciot_c/src/esp8266
COMPONENT_SRCDIRS += $(CIOT_PATH)/ciot_c/src/nanopb
COMPONENT_SRCDIRS += $(CIOT_PATH)/ciot_c/src/protos/ciot/proto/v2
COMPONENT_SRCDIRS += $(CIOT_PATH)/ciot_c/src/nanomodbus
