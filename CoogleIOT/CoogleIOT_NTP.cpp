#include "CoogleIOT_NTP.h"

CoogleIOT_NTP& CoogleIOT_NTP::setWifiManager(CoogleIOT_Wifi *_wifi)
{
	WiFiManager = _wifi;
	return *this;
}

CoogleIOT_NTP& CoogleIOT_NTP::setLogger(CoogleIOT_Logger *_logger)
{
	logger = _logger;
	return *this;
}

CoogleIOT_NTP& CoogleIOT_NTP::setOffsetSeconds(int offset)
{
	offsetSeconds = offset;
	return *this;
}

CoogleIOT_NTP& CoogleIOT_NTP::setDaylightOffsetSeconds(int offset)
{
	daylightOffsetSecs = offset;
	return *this;
}

bool CoogleIOT_NTP::sync()
{
	connectAttempts = 0;

	if(!WiFiManager) {
		return false;
	}

	if(!WiFiManager->connected()) {
		return false;
	}

	if(attemptingSync) {
		return false;
	}

	configTime(offsetSeconds, daylightOffsetSecs, CIOT_NTP_SERVERS);

	attemptingSync = true;

	os_timer_arm(&connectTimer, COOGLEIOT_NTP_SYNC_TIMEOUT, false);

	return true;
}

time_t CoogleIOT_NTP::getNow()
{
	return time(nullptr);
}

bool CoogleIOT_NTP::active()
{
	// This is kind of hacky but if we don't have an epoch that makes sense, it's not active.
	return time(nullptr) > 1000000000;
}

void CoogleIOT_NTP::loop()
{
	if(connectTimerTick) {
		connectTimerTick = false;
		connectAttempts++;

		if(connectAttempts > COOGLEIOT_NTP_MAX_ATTEMPTS) {
			if(logger)
				logger->error("Failed to synchronize local time with NTP");
			attemptingSync = false;
		} else {
			if(!active()) {
				if(logger)
					logger->logPrintf(DEBUG, "NTP Sync attempt #%d", connectAttempts);
				os_timer_arm(&connectTimer, COOGLEIOT_NTP_SYNC_TIMEOUT, false);
			} else {
				now = time(nullptr);
				if(logger)
					logger->info("Time successfully synchronized with NTP server");
				attemptingSync = false;
			}
		}
	}

	if(!active() && !attemptingSync) {
		if(WiFiManager) {
			if(WiFiManager->connected()) {
				if(logger)
					logger->debug("No NTP Sync active, retrying sync");

				sync();
			}
		}
	}

}

extern "C" void __***REMOVED***iot_ntp_connect_timer_callback(void *self)
{
	CoogleIOT_NTP *obj = static_cast<CoogleIOT_NTP *>(self);
	obj->connectTimerTick = true;
}

bool CoogleIOT_NTP::initialize()
{
	if(logger)
		logger->info("Initializing NTP");

	os_timer_setfn(&connectTimer, __***REMOVED***iot_ntp_connect_timer_callback, this);

	if(WiFiManager) {
		if(WiFiManager->connected()) {
			sync();
		}
	}

	return true;
}
