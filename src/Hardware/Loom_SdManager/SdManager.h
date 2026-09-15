#pragma once

#include <OPEnS_RTC.h>
#include <SPI.h>
#include <SdFat.h>

#include "Module.h"

#define SD_PATH_MAX 256U
#define SD_DEVICE_MAX 32U

/**
 * Tracks the hypnos version and matches the version with the correct chip
 * select pin
 */
enum HypnosVersion { V3_2 = 10U, V3_3 = 11U, ADALOGGER = 4U };

/**
 * Class used to manage interaction with SD card read/write on the Hypnos board
 *
 * @author Will Richards
 */
class SdManager {
  public:
    /**
     * SDManager Constructor
     *
     * @param hypnosVersion Version of hypnos board in use
     * @param deviceName Name of device in manager
     */
    SdManager(HypnosVersion hypnosVersion, const char *deviceName);

    /**
     * Initialize the SD card
     */
    bool initialize();

    bool isInitialized(void) { return initialized; }

    /**
     * Get pointer to debug log file
     * Be sure to call logFile->close() when you're done with it
     * @return Pointer to file, nullptr if error occurred
     */
    File *getLogFile(void);

    /**
     * Get pointer to debug log file
     * Be sure to call csvFile->close() when you're done with it
     * @return Pointer to file, nullptr if error occurred
     */
    File *getCsvFile(void);

  private:
    SdFat sd;
    char deviceName[SD_DEVICE_MAX];
    bool initialized;

    void openFile(File *file, const char *path);

    File logFile;
    File csvFile;

    /* SPI chip select pin for the SD card module */
    uint8_t pinChipSelect;

    /* Path to CSV file that data is being logged to */
    char pathCsv[SD_PATH_MAX];

    /* Path to debug log file */
    char pathDebug[SD_PATH_MAX];

    /* Populate the active file names with unique paths */
    bool initFileNames();
};
