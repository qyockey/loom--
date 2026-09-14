#pragma once

#include <cstdint>
#include <OPEnS_RTC.h>

#include "Sensors/I2C/Loom_DS3231/Loom_DS3231.h"
#include "Hardware/Loom_PowerRail/Loom_PowerRail.h"
#include "Hardware/Loom_SdManager/SdManager.h"
#include "Module.h"

// Maximum number of modules the manager can manage
#ifndef LOOM_MAX_MODULES
#define LOOM_MAX_MODULES 8
#endif

// Serial interface baud rate
#ifndef BAUD_RATE
#define BAUD_RATE 115200U
#endif

// Wait up to 20s for Serial
#ifndef LOOM_SERIAL_TIMEOUT_MS
#define LOOM_SERIAL_TIMEOUT_MS 20000U
#endif

struct PacketData {
    uint32_t number;
};

/**
 * Unifies all the various sensors to allow for collection in unison
 * This class manages the JSON document store of all sensor information
 *
 * @author Will Richards
 */
class Manager {
  public:
    /**
     * Constructs a new Manager
     * @param packet Pointer to latest measured packet
     * @param devName Device name to provided for logging purposes
     * @param instanceNum Instance number for logging purposes
     * @param hypnosVersion Hypnos board version, controls SD chip select pin
     */
    Manager(
        struct PacketData *packet,
        const char *devName,
        uint32_t instanceNum,
        HypnosVersion hypnosVersion = HypnosVersion::V3_3
    );

    /**
     * Registers a new sub-module to be controlled by the manager (Used on sensors so measure and
     * package calls can all be called at once)
     * @param moduleInst Pointer to an object that inherits from Module that we want to add
     */
    void registerModule(Module *moduleInst);

    /**
     * Start the serial interface.  Abort if the operation exceeds waitDuration
     * (in ms) so that field deployments don't hang while waiting for a serial
     * connection.
     * @param timeoutMillis Maximum time to wait in milliseconds
     */
    void beginSerial(uint64_t timeoutMillis = LOOM_SERIAL_TIMEOUT_MS);

    /**
     * Set the configuration for the power rails when waking up from sleep
     *
     * NOTE: 3V rail should always be on while awake.
     * Because the 3V rail pulls up SDA and SCL, any attempted I2C transmission
     * while the rail is off will fail and cause the device to hang.
     * This should not be a problem under normal circumstances.
     *
     * @param config The desired configuration while the device is awake
     * See namespace RailState in Loom_PowerRail.h.
     */
    void setWakeConfiguration(struct PowerRailConfig config);

    /**
     * Set the configuration for the power rails when going to sleep
     * @param config The desired configuration while the device is asleep
     * See namespace RailState in Loom_PowerRail.h.
     */
    void setSleepConfiguration(struct PowerRailConfig config);

    /**
     * Calls the initialization function on all added modules
     */
    void initialize();

    /**
     *  Calls the measure function to pull data from the sensors on all added modules
     */
    void measure();

    /**
     * Prints out the current measured data to the Serial bus
     */
    void displayData();

    /**
     * Writes the current measured data to the SD card
     */
    void logToSd();

    /**
     * Drops the Feather M0 and Hypnos board into a low power sleep waiting for
     * an interrupt to wake it up and pull it out of sleep.
     * This is much more power efficient for long delays than calling
     * manager.pause()
     * @param millis The time that will elapse before the device is woken by
     * the RTC
     * @param waitForSerial Whether to still execution until Serial monitor
     * reconnects after powering up.  This takes about 2 seconds.
     */
    void sleep(uint32_t millis, bool waitForSerial = false);

    /**
     * Gets the current device name set by the user
     * @return current device name
     */
    const char *getDeviceName() { return deviceName; };

    /**
     * Gets the current device instance number
     * @return current device instance number
     */
    int getInstanceNum() { return instanceNumber; };

    /**
     * Get the unique serial number of the Feather m0
     * @return Unique serial number
     */
    const char *getSerialNum() { return serialNum; };

    /**
     * Pause execution for a specified length of time
     * @param ms Time to wait for in milliseconds
     */
    void pause(const uint32_t ms) const;

  private:
    /**
     * Clean up modules before cutting power
     */
    void powerDown();

    /**
     * Reinitialize modules after power restored
     */
    void powerUp();

    // Name and instance of the device
    const char *deviceName;
    uint32_t instanceNumber;

    struct PacketData *packet;

    Loom_DS3231 rtcExternal;

    // List of modules that have been added to the stack
    Module *modules[LOOM_MAX_MODULES];

    // Number of modules registered for measurement
    uint8_t numRegisteredModules;

    Loom_PowerRail rails;
    SdManager sd;
    void writeCsvHeader();

    // Serial number unique to device
    void readSerialNum();
    char serialNum[32 + 1];
};
