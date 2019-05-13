#ifndef APP_LOGGER_H_
#define APP_LOGGER_H_

#ifndef LOG_INFO
#define LOG_INFO(msg) _ciot_log->info(msg);
#endif

#ifndef LOG_WARN
#define LOG_WARN(msg) _ciot_log->warn(msg);
#endif

#ifndef LOG_ERROR
#define LOG_ERROR(msg) _ciot_log->error(msg);
#endif

#ifndef LOG_CRITICAL
#define LOG_CRITICAL(msg) _ciot_log->critical(msg);
#endif

#ifndef LOG_PRINTF
#define LOG_PRINTF(severity, format, ...) _ciot_log->logPrintf(severity, format, __VA_ARGS__)
#endif

#ifndef LOG_DEBUG
#define LOG_DEBUG(msg) _ciot_log->debug(msg);
#endif

#endif
