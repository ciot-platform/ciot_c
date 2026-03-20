CIOT_PATH := $(subst \,/,$(CIOT_PATH))

PB_CIOT_PATH := $(CIOT_PATH)/ciot_protos
PB_OUT := .
PB_MYPY_OUT := .

gen-protos:
	protoc --python_out=$(PB_OUT) --mypy_out=$(PB_MYPY_OUT) $(PB_CIOT_PATH)/ciot/proto/v2/*.proto --proto_path=$(PB_CIOT_PATH)

.PHONY: gen-ciot gen-hg gen-pcm gen-protos