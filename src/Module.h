#pragma once

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
};
