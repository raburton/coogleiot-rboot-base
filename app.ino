/*
  +----------------------------------------------------------------------+
  | Coogle IOT Skeleton                                                  |
  +----------------------------------------------------------------------+
  | Copyright (c) 2017-2019 John Coggeshall                              |
  +----------------------------------------------------------------------+
  | Licensed under the Apache License, Version 2.0 (the "License");      |
  | you may not use this file except in compliance with the License. You |
  | may obtain a copy of the License at:                                 |
  |                                                                      |
  | http://www.apache.org/licenses/LICENSE-2.0                           |
  |                                                                      |
  | Unless required by applicable law or agreed to in writing, software  |
  | distributed under the License is distributed on an "AS IS" BASIS,    |
  | WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or      |
  | implied. See the License for the specific language governing         |
  | permissions and limitations under the License.                       |
  +----------------------------------------------------------------------+
  | Authors: John Coggeshall <john@thissmarthouse.com>                   |
  +----------------------------------------------------------------------+
*/

/**
 * Arduino libs must be included in the .ino file to be detected by the build system
 */
#include <FS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <Hash.h>
#include <PubSubClient.h>
#include "ArduinoJson.h"
#include <stdint.h>
#include "app.h"

bool ota_ready = false;

void onMQTTCommand(const char *topic, byte *payload, unsigned int length)
{
	return;
}

void onMQTTConnect()
{
	LOG_PRINTF(DEBUG, "Subscribed to %s", STRINGIZE_VALUE_OF(SET_TOPIC));
	mqtt->subscribe(STRINGIZE_VALUE_OF(SET_TOPIC));
}

void setupSerial()
{
	if(Serial) {
		return;
	}

	Serial.begin(SERIAL_BAUD);

	for(int i = 0; (i < 500000) && !Serial; i++) {
		yield();
	}


	Serial.printf(APP_NAME " v%s (%s) (built: %s %s)\r\n", _BuildInfo.src_version, _BuildInfo.env_version, _BuildInfo.date, _BuildInfo.time);
}

void setupMQTT()
{
	mqttManager = new CoogleIOT_MQTT;
	mqttManager->setLogger(_ciot_log);
	mqttManager->setHostname(STRINGIZE_VALUE_OF(MQTT_SERVER));
	mqttManager->setPort(MQTT_PORT);
	mqttManager->setWifiManager(WiFiManager);
	mqttManager->initialize();
	mqttManager->setConnectCallback(onMQTTConnect);
	mqttManager->connect();

	mqtt = mqttManager->getClient();
	mqtt->setCallback(onMQTTCommand);
}

void onNewFirmware()
{
	uint8_t newRom;

	LOG_INFO("New Firmware available");
	LOG_INFO("Current Firmware Details");
	LOG_PRINTF(INFO, APP_NAME " v%s (%s) (built: %s %s)\r\n", _BuildInfo.src_version, _BuildInfo.env_version, _BuildInfo.date, _BuildInfo.time);

	newRom = (rboot_get_current_rom() == 0) ? 1 : 0;

	LOG_PRINTF(INFO, "Switching to ROM #%d", newRom);

	rboot_set_current_rom(newRom);
	system_restart();
}

void onNTPReady()
{
	setupOTA();
	ota_ready = true;
}

void setupNTP()
{
    NTPManager = new CoogleIOT_NTP;
	NTPManager->setLogger(_ciot_log);
    NTPManager->setWifiManager(WiFiManager);
    NTPManager->setReadyCallback(onNTPReady);
    NTPManager->initialize();

}

void setupLogging()
{
	setupSerial();
    _ciot_log = new CoogleIOT_Logger(&Serial);
    _ciot_log->initialize();

}

void setupWiFi()
{
    WiFiManager = new CoogleIOT_Wifi;
    WiFiManager->setLogger(_ciot_log);
    WiFiManager->setRemoteAPName(STRINGIZE_VALUE_OF(APP_SSID));
    WiFiManager->setRemoteAPPassword(STRINGIZE_VALUE_OF(APP_PASS));

    WiFiManager->initialize();
}

void setupOTA()
{
	otaManager = new CoogleIOT_OTA;
	otaManager->setLogger(_ciot_log);
	otaManager->setWifiManager(WiFiManager);
	otaManager->setNTPManager(NTPManager);
	otaManager->setCurrentVersion(_BuildInfo.src_version);
	otaManager->setOTAManifestEndpoint("https://192.168.1.50:4443/ota-manifest.json");
	otaManager->setOTACompleteCallback(onNewFirmware);
	otaManager->initialize();
}

void logSetupInfo()
{
	FSInfo fs_info;
	LOG_PRINTF(INFO, APP_NAME " v%s (%s) (built: %s %s)\r\n", _BuildInfo.src_version, _BuildInfo.env_version, _BuildInfo.date, _BuildInfo.time);

	if(!SPIFFS.begin()) {
		LOG_ERROR("Failed to start SPIFFS file system!");
	} else {
		SPIFFS.info(fs_info);
		LOG_INFO("SPIFFS File System");
		LOG_INFO("-=-=-=-=-=-=-=-=-=-");
		LOG_PRINTF(INFO, "Total Size: %d byte(s)\nUsed: %d byte(s)\nAvailable: ~ %d byte(s)", fs_info.totalBytes, fs_info.usedBytes, fs_info.totalBytes - fs_info.usedBytes);
	}
}

void setup()
{
    randomSeed(micros());

    setupLogging();

	logSetupInfo();

    setupWiFi();
    setupNTP();
    setupMQTT();

    // Give the logger an NTP Manager so it can record timestamps with logs
    _ciot_log->setNTPManager(NTPManager);
}

void loop()
{
	WiFiManager->loop();
	NTPManager->loop();
	mqttManager->loop();

	if(ota_ready) {
		otaManager->loop();
	}

	//LOG_PRINTF(DEBUG, "Heap Size: %d", ESP.getFreeHeap());

}
