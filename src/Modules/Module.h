#pragma once

#include <SdFat.h>
#include "Hardware/Loom_Multiplexer/Loom_Multiplexer.h"

/**
 *  General overarching interface to provide basic unified functionality
 *
 *  @author Will Richards
 */
class Module {
  public:
    Module() {};

    // Generic calls to unify interaction with different sensor implementations
    virtual void initialize() = 0;  // Initialize all functionality of the sensor
    virtual void measure() = 0;     // Collect data from the sensor
    virtual void displayData() = 0; // Display measured data over serial interface
    virtual void powerDown() = 0;   // Cleanup before cutting power
    virtual void powerUp() = 0;     // Initialize module after power restored

    virtual void writeCsvHeader1(File *csv) = 0;
    virtual void writeCsvHeader2(File *csv) = 0;
    virtual void writeCsvBody(File *csv) = 0;

    bool usingMux(void) {
        return (muxPort > 0);
    }

    uint8_t getMuxPort(void) {
        return (uint8_t) muxPort;
    }

  protected:
    // For modules connected via TCA9548A I²C multiplexer
    int8_t muxPort = -1;
};
