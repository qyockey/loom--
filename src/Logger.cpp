#include <cstdint>
#include <cstdio>
#include <cstring>

#include "Logger.h"
#include "Sensors/I2C/Loom_DS3231/Loom_DS3231.h"
#include "Hardware/Loom_SdManager/SdManager.h"

SdManager *Logger::sdInst = nullptr;
Loom_DS3231 *Logger::rtcInst = nullptr;

void Logger::initialize(SdManager *sd, Loom_DS3231 *rtc) {
    sdInst = sd;
    rtcInst = rtc;

    if (sd == nullptr) {
        WARNING("SD Manager instance is NULL, Logger cannot write to SD card");
        WARNING("Log messages will appear ONLY on Serial termial");
    }

    if (rtc == nullptr) {
        WARNING("RTC instance is NULL, Logger cannot add timestamps");
    }
}

void Logger::log(Print *out, LogContext *log, const char *msg) {
    // Write time if available
    if (rtcInst != nullptr && rtcInst->isInitialized()) {
        out->printf("[%s] ", rtcInst->getCurrentTimeUtcIsoFormat());
    }

    out->printf(
        "[%s] [%s:%s:%lu] ",
        log->level, getFileBasename(log->file), log->func, log->lineNum, msg
    );

    out->println(msg);
}

void Logger::genericLog(LogContext *log, const char *msg) {
    if (Serial && !log->silent) {
        Logger::log(&Serial, log, msg);
    }

    if (sdInst != nullptr && sdInst->isInitialized()) {
        File *logFile = sdInst->getLogFile();
        Logger::log(logFile, log, msg);
        logFile->close();
    }
}

const char *Logger::getFileBasename(const char *fullPath) {
    // Check if directory separator is backslash (compiled on Windows)
    const char *basename = strrchr(fullPath, '\\');
    if (basename != nullptr) {
        return basename + 1; // Skip '\\' character
    }

    // Check if directory separator is forward slash (compiled on Linux/Mac)
    basename = strrchr(fullPath, '/');
    if (basename != nullptr) {
        return basename + 1; // Skip '/' character
    }

    // No directory separators present
    return fullPath;
}
