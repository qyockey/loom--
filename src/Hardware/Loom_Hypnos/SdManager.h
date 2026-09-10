#pragma once

#include <OPEnS_RTC.h>
#include <SPI.h>
#include <SdFat.h>

#include "Module.h"

#define SD_PATH_MAX 256U
#define SD_DEVICE_MAX 32U

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
     * @param pinChipSelect SPI chip select pin for the SD card module
     * @param deviceName Name of device in manager
     */
    SdManager(uint8_t pinChipSelect, const char *deviceName);

    /**
     * Initialize the SD card
     */
    bool initialize();

    /**
     * Write a single line to a file
     * @param filename File to write to
     * @param content Message to write
     */
    bool writeLineToFile(const char *filename, const char *content);

    /**
     * Write a single line to the debug log file
     * @param content Message to write
     */
    bool writeDebugLine(const char *content);

  private:
    SdFat sd;
    char deviceName[SD_DEVICE_MAX];
    bool initialized;

    /* SPI chip select pin for the SD card module */
    uint8_t pinChipSelect;

    /* Path to CSV file that data is being logged to */
    char pathCsv[SD_PATH_MAX];

    /* Path to debug log file */
    char pathDebug[SD_PATH_MAX];

    /* Populate the active file names with unique paths */
    bool initFileNames();
};
