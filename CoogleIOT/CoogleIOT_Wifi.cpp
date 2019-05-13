#include "CoogleIOT_Wifi.h"

CoogleIOT_Wifi& CoogleIOT_Wifi::setLogger(CoogleIOT_Logger *_logger)
{
	logger = _logger;
	return *this;
}

char *CoogleIOT_Wifi::getRemoteAPName()
{
	char *retval = (char *)malloc(strlen(remoteAPName) + 1);

	if(strlen(remoteAPName) > 0) {
		memcpy(retval, &remoteAPName[0], strlen(remoteAPName) + 1);
	}

	return retval;
}

char *CoogleIOT_Wifi::getRemoteAPPassword()
{
	char *retval = (char *)malloc(strlen(remoteAPPassword) + 1);

	if(strlen(remoteAPPassword) > 0) {
		memcpy(retval, &remoteAPPassword[0], strlen(remoteAPPassword) + 1);
	}

	return retval;
}

char *CoogleIOT_Wifi::getLocalAPName()
{
	char *retval = (char *)malloc(strlen(localAPName) + 1);

	if(strlen(localAPName) > 0) {
		memcpy(retval, &localAPName[0], strlen(localAPName) + 1);
	}

	return retval;
}

char *CoogleIOT_Wifi::getLocalAPPassword()
{
	char *retval = (char *)malloc(strlen(localAPPassword) + 1);

	if(strlen(localAPPassword)) {
		memcpy(retval, &localAPPassword[0], strlen(localAPPassword) + 1);
	}

	return retval;
}

char *CoogleIOT_Wifi::getRemoteStatus()
{
    switch(WiFi.status()) {
        case WL_CONNECTED:
            return "Connected";
        case WL_NO_SSID_AVAIL:
            return "No SSID Available";
        case WL_CONNECT_FAILED:
            return "Failed to Connect";
        case WL_IDLE_STATUS:
            return "Idle";
        case WL_DISCONNECTED:
        	return "Disconnected";
        default:
            return "Unknown";
    }
}

CoogleIOT_Wifi& CoogleIOT_Wifi::setRemoteAPName(String &name)
{
	return setRemoteAPName(name.c_str());
}

CoogleIOT_Wifi& CoogleIOT_Wifi::setRemoteAPName(const char *name)
{
	memset(&remoteAPName[0], NULL, 33);
	memcpy(&remoteAPName[0], name, (strlen(name) > 32) ? 32 : strlen(name));
	return *this;
}

CoogleIOT_Wifi& CoogleIOT_Wifi::setRemoteAPPassword(String &pass)
{
	return setRemoteAPPassword(pass.c_str());
}

CoogleIOT_Wifi& CoogleIOT_Wifi::setRemoteAPPassword(const char *pass)
{
	memset(&remoteAPPassword[0], NULL, 65);
	memcpy(&remoteAPPassword[0], pass, (strlen(pass) > 64) ? 64 : strlen(pass));
	return *this;
}

CoogleIOT_Wifi& CoogleIOT_Wifi::setLocalAPName(String &name)
{
	return setLocalAPName(name.c_str());
}

CoogleIOT_Wifi& CoogleIOT_Wifi::setLocalAPName(const char *name)
{
	memset(&localAPName[0], NULL, 33);
	memcpy(&localAPName[0], name, (strlen(name) > 32) ? 32 : strlen(name));
	return *this;
}

CoogleIOT_Wifi& CoogleIOT_Wifi::setLocalAPPassword(String &pass)
{
	return setLocalAPPassword(pass.c_str());
}

CoogleIOT_Wifi& CoogleIOT_Wifi::setLocalAPPassword(const char *pass)
{
	memset(&localAPPassword[0], NULL, 65);
	memcpy(&localAPPassword[0], pass, (strlen(pass) > 64) ? 64 : strlen(pass));
	return *this;

}

bool CoogleIOT_Wifi::connect()
{
	if(connected()) {
		return true;
	}

	if(attemptingConnection) {
		return false;
	}

	wifiFailuresCount = 0;

	if(strlen(remoteAPName) == 0) {
		if(logger)
			logger->info("Cannot connect to WiFi as no SSID was provided");
		return false;
	}

	if(logger)
		logger->logPrintf(INFO, "Attempting to Connect to SSID: %s", remoteAPName);

	if(strlen(remoteAPPassword) == 0) {
		if(logger)
			logger->warn("Remote AP is an open network");

		WiFi.begin(remoteAPName, NULL, 0, NULL, true);
	} else {
		WiFi.begin(remoteAPName, remoteAPPassword, 0, NULL, true);
	}

	attemptingConnection = true;

	os_timer_arm(&wifiConnectTimer, COOGLEIOT_WIFI_CONNECT_TIMEOUT, false);

	return true;
}

extern "C" void __***REMOVED***iot_wifi_connect_timer_callback(void *self)
{
	CoogleIOT_Wifi *obj = static_cast<CoogleIOT_Wifi *>(self);
	obj->wifiConnectTimerTick = true;
}

void CoogleIOT_Wifi::loop()
{
	if(wifiConnectTimerTick) {
		wifiConnectTimerTick = false;

		wifiFailuresCount++;

		if(logger)
			logger->logPrintf(INFO, "WiFi Connect Attempt %d: %s", wifiFailuresCount, getRemoteStatus());

		if(wifiFailuresCount > COOGLEIOT_WIFI_MAX_ATTEMPTS) {
			if(logger)
				logger->error("Failed to connect to WiFi, giving up.");
			attemptingConnection = false;
		} else {
			if(!connected()) {
				os_timer_arm(&wifiConnectTimer, COOGLEIOT_WIFI_CONNECT_TIMEOUT, false);
			} else {
				if(logger)
					logger->logPrintf(INFO, "Success! IP Address: %s", WiFi.localIP().toString().c_str());
				attemptingConnection = false;
			}
		}
	}

	if(!connected() && !attemptingConnection) {
		connect();
	}
}

bool CoogleIOT_Wifi::initialize()
{
	if(logger)
		logger->info("Initializing WiFiManager");

	WiFi.disconnect();
	WiFi.setAutoConnect(false);
	WiFi.setAutoReconnect(true);
	WiFi.mode(WIFI_AP_STA);

	os_timer_setfn(&wifiConnectTimer, __***REMOVED***iot_wifi_connect_timer_callback, this);

	connect();
}

bool CoogleIOT_Wifi::connected()
{
	return (WiFi.status() == WL_CONNECTED);
}
