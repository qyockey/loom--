#include <ArduinoJson.h>

#include "SdManager.h"
#include "Logger.h"

SdManager::SdManager(uint8_t pinChipSelect, const char *deviceName)
    :pinChipSelect(pinChipSelect) {
    strncpy(SdManager::deviceName, deviceName, SD_DEVICE_MAX);
}

bool SdManager::initialize() {
    Serial.println("Initializing SD Card...");

    /* Start the SD card with a slow SPI speed */
    if (!sd.begin(pinChipSelect, SD_SCK_MHZ(4))) {
        Serial.println(
            "Failed to Initialize SD Card! SD Card functionality will be "
            "disabled, is there an SD card inserted into the device?"
        );
        return false;
    }

    /* Make a debug folder if it doesn't already exist */
    if (!sd.exists("debug")) {
        sd.mkdir("debug");
    }

    /* Populate standard file names (data CSV, debug log, etc.) */
    if (!initFileNames()) {
        return false;
    }

    initialized = true;
    Serial.println("Successfully initialized SD Card!");
    return true;
}

void SdManager::openFile(File *file, const char *path) {
    /* Print errors to Serial only, don't write to log file */

    if (!initialized) {
        Serial.printf(
            "[ERROR] SD Card not initialized, cannot open file %s\n",
            path
        );
        return;
    }

    *file = sd.open(path, O_RDWR | O_CREAT | O_APPEND);
    if (!file && Serial) {
        Serial.printf("[ERROR] Failed to open file %s\n", path);
        return;
    }
}

File *SdManager::getLogFile(void) {
    openFile(&logFile, pathDebug);
    if (!logFile) {
        return nullptr;
    }

    return &logFile;
}

File *SdManager::getCsvFile(void) {
    openFile(&csvFile, pathCsv);
    if (!csvFile) {
        return nullptr;
    }

    return &csvFile;
}

bool SdManager::initFileNames() {
    /* Try to open the root of the file system so we can read all
     * files in the root directory */
    File root;
    if (!root.open("/", O_RDONLY)) {
        Serial.println("[ERROR] Failed to open file system root on SD Card!");
        return false;
    }

    /* Count all file names in root matching device name */
    File scanningFile;
    uint16_t fileCount = 0;
    while (scanningFile.openNext(&root)) {
        char fName[SD_PATH_MAX];
        scanningFile.getName(fName, SD_PATH_MAX);

        /* Check if the file begins with device name */
        if (strstr(fName, deviceName) == fName) {
            fileCount++;
        }

        scanningFile.close();
    }

    /* Set CSV path as device name followed by a unique number */
    snprintf(pathCsv, SD_PATH_MAX, "%s%u.csv", deviceName, fileCount);
    Serial.printf("Data will be logged to %s\n", pathCsv);

    /* Set unique log file in debug directory */
    snprintf(pathDebug, SD_PATH_MAX, "debug/output_%u.log", fileCount);

    root.close();
    return true;
}
