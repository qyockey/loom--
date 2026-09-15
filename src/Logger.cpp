#include <cstdint>
#include <cstdio>
#include <cstring>

#include "Logger.h"
#include "Hardware/Loom_Hypnos/Loom_Hypnos.h"
#include "Hardware/Loom_Hypnos/SdManager.h"

SdManager *Logger::sdInst = nullptr;
Loom_Hypnos *Logger::hypnosInst = nullptr;

void Logger::initialize(SdManager *sd, Loom_Hypnos *hypnos) {
    sdInst = sd;
    hypnosInst = hypnos;

    if (sd == nullptr) {
        WARNING("SD Manager instance is NULL, Logger cannot write to SD card");
        WARNING("Log messages will appear ONLY on Serial termial");
    }

    if (hypnosInst == nullptr) {
        WARNING("Hypnos instance is NULL, Logger cannot add timestamps");
    }
}

void Logger::log(Print *out, LogContext *log, const char *msg) {
    // Write time if available
    if (hypnosInst != nullptr && hypnosInst->isRtcInitialized()) {
        out->printf("[%sZ] ", hypnosInst->getCurrentTimeUtc().text());
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
