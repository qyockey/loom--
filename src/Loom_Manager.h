#pragma once

#include <cstdint>

#include "Module.h"

#define BAUD_RATE 115200   // Serial interface baud rate

// Maximum number of modules the manager can manage
#define LOOM_MAX_MODULES 16

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
     * Start the Serial interface
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
     * Pause execution for a specified length of time
     * @param ms Time to wait for in milliseconds
     */
    void pause(const uint32_t ms) const;

  private:
    // List of modules that have been added to the stack
    Module modules[LOOM_MAX_MODULES];
    uint8_t numRegisteredModules;
};
