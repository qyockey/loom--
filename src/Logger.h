#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "Sensors/I2C/Loom_DS3231/Loom_DS3231.h"
#include "Hardware/Loom_SdManager/SdManager.h"

#define OUTPUT_SIZE 2000U

struct LogContext {
    const char *file;
    const char *func;
    unsigned long lineNum;
    bool silent;
    const char *level; // must have static lifetime
};

#define GENERIC_LOG(silent, level, msg)                                                            \
    do {                                                                                           \
        LogContext log{__FILE__, __func__, __LINE__, silent, level};                               \
        Logger::genericLog(log, msg);                                                              \
    } while (false)

#define LOG(msg) GENERIC_LOG(false, "DEBUG", msg)
#define SLOG(msg) GENERIC_LOG(true, "DEBUG", msg)
#define WARNING(msg) GENERIC_LOG(false, "WARNING", msg)
#define ERROR(msg) GENERIC_LOG(false, "ERROR", msg)

#define GENERIC_LOGF(silent, level, msg, ...)                                                      \
    do {                                                                                           \
        LogContext log{__FILE__, __func__, __LINE__, silent, level};                               \
        char buf[OUTPUT_SIZE];                                                                     \
        snprintf_P(buf, sizeof(buf), PSTR(msg), ##__VA_ARGS__);                                    \
        Logger::genericLog(log, buf);                                                              \
    } while (false)

#define LOGF(msg, ...) GENERIC_LOGF(false, "DEBUG", msg, ##__VA_ARGS__)
#define SLOGF(msg, ...) GENERIC_LOGF(true, "DEBUG", msg, ##__VA_ARGS__)
#define WARNINGF(msg, ...) GENERIC_LOGF(false, "WARNING", msg, ##__VA_ARGS__)
#define ERRORF(msg, ...) GENERIC_LOGF(false, "ERROR", msg, ##__VA_ARGS__)

/**
 * Arduino Logger class that allows for standardized log outputs as well as
 * function memory usage summaries to find memory leaks that may lead to
 * unexpected crashing
 *
 * @author Will Richards
 */
class Logger {
  private:
    static SdManager *sdInst;
    static Loom_DS3231 *rtcInst;
    static char logFilePath[100];

    /**
     * Generic log function - prints to Serial and logs to SD
     *
     * @param message The message we want to log
     * @param silent Whether to print to the serial monitor
     */
    static void log(char *message, bool silent);

  public:
    /**
     * Initialize Logger with objects to fulfill logging functionality.
     * SD manager provides logging to file, RTC adds timestamp to log
     * message.
     * @param sd Pointer to SD Manager instance
     * @param hypnos Pointer to Hypnos instance
     */
    static void initialize(SdManager *sd = nullptr, Loom_DS3231 *rtc = nullptr);

    /**
     * Write log message.
     * @param log Log context and configuration.  See LogContext struct
     * definition.
     * @param msg Log message to write.
     */
    static void genericLog(LogContext log, const __FlashStringHelper *msg);
    static void genericLog(LogContext log, const char *msg);

    /**
     * Truncate the __FILE__ output to just show the name instead of the whole
     * path.  Expects dst to be at least as large as src.
     */
    static const char *getFileBasename(const char *full_path);
};
