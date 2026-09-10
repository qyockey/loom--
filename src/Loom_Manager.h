#pragma once

#include <cstdint>
#include <OPEnS_RTC.h>

#include "Hardware/Loom_Hypnos/Loom_Hypnos.h"
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
     * @param devName Device name to provided for logging purposes
     * @param instanceNum Instance number for logging purposes
     * @param hypnosInst Optional instance of hypnos used for sleep
     */
    Manager(
        const char *devName,
        uint32_t instanceNum,
        Loom_Hypnos *hypnosInst = nullptr
    );

    /**
     * Registers a new sub-module to be controlled by the manager (Used on sensors so measure and
     * package calls can all be called at once)
     * @param module Pointer to a class the inherits from Module that we want to add
     */
    void registerModule(Module *module);

    /**
     * Start the serial interface.  Abort if the operation exceeds waitDuration
     * (in ms) so that field deployments don't hang while waiting for a serial
     * connection.
     * @param timeoutMillis Maximum time to wait in milliseconds
     */
    void beginSerial(uint64_t timeoutMillis = LOOM_SERIAL_TIMEOUT_MS);

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
    void display_data();

    /**
     * Drops the Feather M0 and Hypnos board into a low power sleep waiting for
     * an interrupt to wake it up and pull it out of sleep.
     * This is much more power efficient for long delays than calling
     * manager.pause()
     * @param duration The time that will elapse before the device is woken by
     * the RTC
     */
    void sleep(TimeSpan duration);

    /**
     * Gets the current device name set by the user
     * @return current device name
     */
    const char *get_device_name() { return deviceName; };

    /**
     * Gets the current device instance number
     * @return current device instance number
     */
    int get_instance_num() { return instanceNumber; };

    /**
     * Get the unique serial number of the Feather m0
     * @return Unique serial number
     */
    const char *get_serial_num() { return serial_num; };

    /**
     * Pause execution for a specified length of time
     * @param ms Time to wait for in milliseconds
     */
    void pause(const uint32_t ms) const;

  private:
    /**
     * Clean up modules before cutting power
     */
    void power_down();

    /**
     * Reinitialize modules after power restored
     */
    void power_up();

    // List of modules that have been added to the stack
    Module *modules[LOOM_MAX_MODULES];

    // Number of modules registered for measurement
    uint8_t numRegisteredModules;

    // Name and instance of the device
    const char *deviceName;
    uint32_t instanceNumber;
    Loom_Hypnos *hypnosInst = nullptr;

    // Serial number unique to device
    void read_serial_num();
    char serial_num[32 + 1];
};
