#include <cstdint>
#include <cstdio>
#include <cstring>

#include "Logger.h"
#include "Sensors/I2C/Loom_DS3231/Loom_DS3231.h"
#include "Hardware/Loom_SdManager/SdManager.h"

SdManager *Logger::sdInst = nullptr;
Loom_DS3231 *Logger::rtcInst = nullptr;
char Logger::logFilePath[100] = {};

void Logger::log(char *message, bool silent) {
    // If we want to actually print to serial
    if (!silent && Serial) {
        Serial.println(message);
    }

    // Log as long as we have given it a SD card instance
    if (sdInst != nullptr) {
        Logger::sdInst->writeDebugLine(message);
    }
}

void Logger::initialize(SdManager *sd, Loom_DS3231 *rtc) {
    sdInst = sd;
    rtcInst = rtc;

    if (sd == nullptr) {
        WARNING("SD Manager instance is NULL, Logger cannot write to SD card");
        WARNING("Log messages will appear ONLY on Serial termial");
    }
}

void Logger::genericLog(LogContext log, const __FlashStringHelper *msg) {
    char buf[OUTPUT_SIZE];
    strncpy_P(buf, (const char *)msg, OUTPUT_SIZE);
    genericLog(log, buf);
}

void Logger::genericLog(LogContext log, const char *msg) {
    char logMessage[OUTPUT_SIZE];
    int traverse = 0;
    const char *activeFileBasename = getFileBasename(log.file);

    // Write time if available
    if (rtcInst != nullptr) {
        traverse += snprintf_P(
            logMessage + traverse, OUTPUT_SIZE - traverse,
            PSTR("[%sZ] "),
            rtcInst->getCurrentTimeUtc().text()
        );
    }

    // Append context and message
    traverse += snprintf_P(
        logMessage + traverse, OUTPUT_SIZE - traverse,
        PSTR("[%s] [%s:%s:%lu] %s"),
        log.level, activeFileBasename, log.func, log.lineNum, msg
    );

    Logger::log(logMessage, log.silent);
}

const char *Logger::getFileBasename(const char *full_path) {
    // Check if directory separator is backslash
    const char *basename = strrchr(full_path, '\\');
    if (basename != nullptr) {
        return basename + 1; // Skip '\\' character
    }

    // Check if directory separator is forward slash
    basename = strrchr(full_path, '/');
    if (basename != nullptr) {
        return basename + 1; // Skip '/' character
    }

    // Default return full file path
    return full_path;
}
