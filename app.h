#ifndef __APP_H_
#define __APP_H_

#include "buildinfo.h"
#include <rboot.h>
#include <rboot-api.h>
#include "CoogleIOT_Logger.h"
#include "CoogleIOT_Wifi.h"
#include "CoogleIOT_NTP.h"
#include "logger.h"

#ifndef SERIAL_BAUD
#define SERIAL_BAUD 115200
#endif

#ifndef DEBUG_HOSTNAME
#define DEBUG_HOSTNAME "***REMOVED***-light"
#endif

struct app_config {
	char ssid[33];
	char pass[64];
	char ota_endpoint[256];
	char ota_token[256];
};

class CoogleIOT_Logger;
class CoogleIOT_Wifi;
class CoogleIOT_NTP;

CoogleIOT_Logger *_ciot_log;
CoogleIOT_Wifi *WiFiManager;
CoogleIOT_NTP *NTPManager;

#endif
