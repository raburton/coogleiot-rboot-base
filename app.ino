/**
 * Arduino libs must be included in the .ino file to be detected by the build system
 */
#include <FS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Hash.h>

#include "app.h"

#ifdef COOGLEIOT_WITH_REMOTEDEBUG
RemoteDebug COOGLEIOT_REMOTEDEBUG_INSTANCE_NAME;
#endif

#ifdef COOGLEIOT_WITH_REMOTEDEBUG
void handleDebugCommand()
{
	String lastCmd = COOGLEIOT_REMOTEDEBUG_INSTANCE_NAME.getLastCommand();
}
#endif

void setupSerial()
{
	if(Serial) {
		return;
	}

	Serial.begin(SERIAL_BAUD);

	for(int i = 0; (i < 500000) && !Serial; i++) {
		yield();
	}

	Serial.printf("App v%s (%s) (built: %s)\r\n", _BuildInfo.src_version, _BuildInfo.env_version, _BuildInfo.date);
}

void setupNTP()
{
    NTPManager = new CoogleIOT_NTP;
	NTPManager->setLogger(_ciot_log);
    NTPManager->setWifiManager(WiFiManager);
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
    WiFiManager->setRemoteAPName("***REMOVED***");
    WiFiManager->setRemoteAPPassword("***REMOVED***");

    WiFiManager->initialize();
}

void setup() {
    randomSeed(micros());

    setupLogging();

    LOG_PRINTF(INFO, "App v%s (%s) (built: %s)", _BuildInfo.src_version, _BuildInfo.env_version, _BuildInfo.date);

    setupWiFi();
    setupNTP();

#ifdef COOGLEIOT_WITH_REMOTEDEBUG
    COOGLEIOT_REMOTEDEBUG_INSTANCE_NAME.begin(DEBUG_HOSTNAME);
#ifdef COOGLEIOT_REMOTEDEBUG_PASSWORD
    COOGLEIOT_REMOTEDEBUG_INSTANCE_NAME.setPassword(COOGLEIOT_REMOTEDEBUG_PASSWORD);
#endif
    COOGLEIOT_REMOTEDEBUG_INSTANCE_NAME.showProfiler(true);
    COOGLEIOT_REMOTEDEBUG_INSTANCE_NAME.showColors(true);
    COOGLEIOT_REMOTEDEBUG_INSTANCE_NAME.setSerialEnabled(false);

    COOGLEIOT_REMOTEDEBUG_INSTANCE_NAME.setCallBackProjectCmds(&handleDebugCommand);
#endif

    // Give the logger an NTP Manager so it can record timestamps with logs
    _ciot_log->setNTPManager(NTPManager);

}

void loop()
{

#ifdef COOGLEIOT_WITH_REMOTEDEBUG
	/*
	 * If we have remotedebug available and an IP disable the stream
	 * logging in favor of that
	 */
	if(_ciot_log->streamEnabled()) {
		if(WiFiManager->connected()) {
			LOG_PRINTF(INFO, "Disabling Serial Logging in Favor of Remote Debugger. IP Address is %s", WiFi.localIP().toString().c_str());
			_ciot_log->disableStream();
		}
	}
#endif

	WiFiManager->loop();
	NTPManager->loop();

	LOG_PRINTF(DEBUG, "Heap Size: %d", ESP.getFreeHeap());
	delay(1000);

#ifdef COOGLEIOT_WITH_REMOTEDEBUG
	COOGLEIOT_REMOTEDEBUG_INSTANCE_NAME.handle();
#endif

}
