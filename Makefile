CIOT_PATH := $(subst \,/,$(CIOT_PATH))

PB_CIOT_PATH := $(CIOT_PATH)/ciot_protos
PB_CIOT_OPTIONS := src/protos/global.options
PB_SPARKPLUG_B_OPTIONS := src/protos/sparkplug_b.options
PB_OUT := src\protos

gen:
	generator\protoc --nanopb_opt=" -f $(PB_CIOT_OPTIONS) --c-style" --nanopb_out=$(PB_OUT) $(PB_CIOT_PATH)/ciot/proto/v2/*.proto --proto_path=$(PB_CIOT_PATH)
	generator\protoc --nanopb_opt=" -f $(PB_SPARKPLUG_B_OPTIONS) --c-style" --nanopb_out=$(PB_OUT) $(PB_CIOT_PATH)/sparkplug-b/proto/v1/*.proto --proto_path=$(PB_CIOT_PATH)
	del $(PB_OUT)\ciot\proto\v2\iface_manager.pb.c
	del $(PB_OUT)\ciot\proto\v2\iface_manager.pb.h

gen-sparkplugb:
	generator\protoc --nanopb_opt=" -f $(PB_SPARKPLUG_B_OPTIONS) --c-style" --nanopb_out=$(PB_OUT) $(PB_CIOT_PATH)/sparkplug-b/proto/v1/*.proto --proto_path=$(PB_CIOT_PATH)


grpc:
	powershell -Command "dotnet clean '$(CIOT_PATH)\ciot_cs\Ciot.Protos\Ciot.Protos.csproj' -v:diag"
	powershell -Command dotnet build '$(CIOT_PATH)\ciot_cs\Ciot.Protos\Ciot.Protos.csproj' -v:diag"
	powershell -Command dotnet run --project ..\ciot_cs\Ciot.Grpc\Ciot.Grpc.csproj --configuration Debug -v:diag"

help:
	@echo gen-ciot: generate ANSI-C proto files
	