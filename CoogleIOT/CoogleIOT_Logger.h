#ifndef COOGLEIOT_LOGGER_H
#define COOGLEIOT_LOGGER_H

#ifndef COOGLEIOT_LOGGER_LOGFILE
#define COOGLEIOT_LOGGER_LOGFILE "/app-log.txt"
#endif

#ifndef COOGLEIOT_LOGGER_MAXSIZE
#define COOGLEIOT_LOGGER_MAXSIZE 32768 // 32k
#endif

//#define COOGLEIOT_WITH_REMOTEDEBUG

#include <time.h>
#include <FS.h>
#include "CoogleIOT_NTP.h"
#include "CoogleIOT_Logger_Severity.h"

#ifdef COOGLEIOT_WITH_REMOTEDEBUG
#include <RemoteDebug.h>

#ifndef COOGLEIOT_REMOTEDEBUG_INSTANCE_NAME
#define COOGLEIOT_REMOTEDEBUG_INSTANCE_NAME Debug
#endif

#endif



extern HardwareSerial Serial;

class CoogleIOT_NTP;

class CoogleIOT_Logger
{
	public:
		CoogleIOT_Logger(Stream *);
		CoogleIOT_Logger();
		~CoogleIOT_Logger();

		bool initialize();

		CoogleIOT_Logger& disableStream();
		CoogleIOT_Logger& setStream(Stream *);
		bool streamEnabled();

		CoogleIOT_Logger& setNTPManager(CoogleIOT_NTP *);

		CoogleIOT_Logger& warn(String &);
		CoogleIOT_Logger& error(String &);
		CoogleIOT_Logger& critical(String &);
		CoogleIOT_Logger& log(String &, CoogleIOT_Logger_Severity);
		CoogleIOT_Logger& logPrintf(CoogleIOT_Logger_Severity, const char *, ...);
		CoogleIOT_Logger& debug(String &);
		CoogleIOT_Logger& info(String &);
		char *buildLogMsg(String &, CoogleIOT_Logger_Severity);

		CoogleIOT_Logger& warn(const char *);
		CoogleIOT_Logger& error(const char *);
		CoogleIOT_Logger& critical(const char *);
		CoogleIOT_Logger& log(const char *, CoogleIOT_Logger_Severity);
		CoogleIOT_Logger& debug(const char*);
		CoogleIOT_Logger& info(const char *);
		char *buildLogMsg(const char *, CoogleIOT_Logger_Severity);

		char *getLogs();
		File& getLogFile();
		char *getTimestampAsString();

	private:
		File logFile;
		Stream *_stream;
		CoogleIOT_NTP *ntpManager = NULL;
};
#endif
