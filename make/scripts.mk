CIOT_PATH := $(subst \,/,$(CIOT_PATH))

mqtt-translator:
	@echo "Running MQTT translator with host=$(host), port=$(port), topic=$(topic), username=$(username), password=$(password)"
	@python $(CIOT_PATH)/scripts/mqtt_translator.py --host $(host) --port $(port) --topic $(topic) --username $(username) --password $(password) --topic $(topic)