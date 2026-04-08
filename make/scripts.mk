CIOT_PATH := $(subst \,/,$(CIOT_PATH))

mqtt-translator:
	@echo "Running MQTT translator with host=$(MQTT_HOST), port=$(MQTT_PORT), topic=$(MQTT_TOPIC), username=$(MQTT_USERNAME), password=$(MQTT_PASSWORD)"
	@python $(CIOT_PATH)/ciot_c/scripts/mqtt_translator.py --host $(MQTT_HOST) --port $(MQTT_PORT) --topic $(MQTT_TOPIC) --username $(MQTT_USERNAME) --password $(MQTT_PASSWORD)