#ifndef COOGLEIOT_NTP_H
#define COOGLEIOT_NTP_H

#include <time.h>
#include "CoogleIOT_Logger.h"
#include "CoogleIOT_Wifi.h"

#ifndef CIOT_NTP_SERVERS
#define CIOT_NTP_SERVERS  "pool.ntp.org", "time.nist.gov", "time.google.com"
#endif

#ifndef COOGLEIOT_NTP_MAX_ATTEMPTS
#define COOGLEIOT_NTP_MAX_ATTEMPTS 10
#endif

#ifndef COOGLEIOT_NTP_SYNC_TIMEOUT
#define COOGLEIOT_NTP_SYNC_TIMEOUT 1000
#endif

#ifndef COOGLEIOT_NTP_DEFAULT_TZ_OFFSET
#define COOGLEIOT_NTP_DEFAULT_TZ_OFFSET -18000 // EST
#endif

#ifndef COOGLEIOT_NTP_DEFAULT_DAYLIGHT_OFFSET
#define COOGLEIOT_NTP_DEFAULT_DAYLIGHT_OFFSET 3600 // 1 hour
#endif

class CoogleIOT_Logger;
class CoogleIOT_Wifi;

class CoogleIOT_NTP
{
	public:
		CoogleIOT_NTP& setWifiManager(CoogleIOT_Wifi *);
		CoogleIOT_NTP& setLogger(CoogleIOT_Logger *);

		bool sync();
		bool initialize();
		void loop();

		CoogleIOT_NTP& setOffsetSeconds(int);
		CoogleIOT_NTP& setDaylightOffsetSeconds(int);

		time_t getNow();
		bool active();
		bool connectTimerTick = false;

	private:
		time_t now;

		int offsetSeconds = COOGLEIOT_NTP_DEFAULT_TZ_OFFSET;
		int daylightOffsetSecs = COOGLEIOT_NTP_DEFAULT_DAYLIGHT_OFFSET;

		os_timer_t connectTimer;
		int connectAttempts = 0;
		bool attemptingSync = false;

		CoogleIOT_Wifi *WiFiManager = NULL;
		CoogleIOT_Logger *logger = NULL;
};

#endif
