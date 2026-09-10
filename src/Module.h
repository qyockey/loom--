#pragma once

enum ModuleTag {
    MODULE_RTC,
    MODULE_ANALOG,
    MODULE_DIGITAL,
    MODULE_RANDOM,
    MODULE_TSL2591,
    MODULE_STEMMA,
    MODULE_AS7265X,
};

/**
 *  General overarching interface to provide basic unified functionality
 *
 *  @author Will Richards
 */
class Module {
  public:
    Module() {};

    // Generic calls to unify interaction with different sensor implementations
    virtual void initialize() = 0;   // Initialize all functionality of the sensor
    virtual void measure() = 0;      // Collect data from the sensor
    virtual void display_data() = 0; // Display measured data over serial interface
    virtual void power_down() = 0;   // Cleanup before cutting power
    virtual void power_up() = 0;     // Initialize module after power restored
};
