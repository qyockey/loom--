#pragma once

#include <cstdint>
#include <vector>

#include "Module.h"

#define BAUD_RATE 115200U   // Serial interface baud rate
#define LOOM_SERIAL_WAIT_MS 20000U  // Wait up to 20s for Serial

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
     */
    Manager();

    /**
     * Registers a new sub-module to be controlled by the manager (Used on sensors so measure and
     * package calls can all be called at once)
     * @param module Pointer to a class the inherits from Module that we want to add
     */
    void registerModule(Module *module);

    /**
     * Start the serial interface.  Abort if the operation exceeds
     * LOOM_SERIAL_WAIT_MS so that field deployments don't hang while waiting
     * for a serial connection.
     */
    void beginSerial();

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
     * Clean up modules before cutting power
     */
    void power_down();

    /**
     * Reinitialize modules after power restored
     */
    void power_up();

    /**
     * Pause execution for a specified length of time
     * @param ms Time to wait for in milliseconds
     */
    void pause(const uint32_t ms) const;

  private:
     // List of modules that have been added to the stack
    std::vector<Module *> modules;
};
