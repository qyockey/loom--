#include <ArduinoJson.h>

#include "SdManager.h"
#include "Logger.h"

SdManager::SdManager(uint8_t pinChipSelect, const char *deviceName)
    :pinChipSelect(pinChipSelect) {
    strncpy(SdManager::deviceName, deviceName, SD_DEVICE_MAX);
}

bool SdManager::initialize() {
    Serial.println("Initializing SD Card...");

    /* Start the SD card with the fastest SPI speed */
    if (!sd.begin(pinChipSelect, SD_SCK_MHZ(50))) {
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

bool SdManager::writeLineToFile(const char *filename, const char *content) {
    if (!initialized) {
        Serial.printf(
            "SD Card not initialized, cannot write to file %s\n",
            filename
        );
        return false;
    }

    /* Open the given file for writing, appending to existing data */
    File file = sd.open(filename, O_RDWR | O_CREAT | O_APPEND);
    if (!file) {
        if (Serial) {
            Serial.printf("[ERROR] Failed to open file %s\n", filename);
        }
        return false;
    }

    file.println(content);
    file.close();
    return true;
}

bool SdManager::writeDebugLine(const char *content) {
    return writeLineToFile(pathDebug, content);
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
