#pragma once

#include <Arduino.h>

#include "Loom_Manager.h"
#include "Module.h"

#define PIN_VBAT 7
#define ADC_RESOLUTION_BITS 12
#define ADC_MAX_CODE ((1 << ADC_RESOLUTION_BITS) - 1)
#define ADC_VREF 3.3F
#define MAX_ANALOG_PINS 8

/* Contain all the information regarding the analog pin that we want to use */
struct AnalogMapping {
    bool active;
    uint16_t analogCode;
    uint16_t analogMv;
};

/**
 * Used to read analog voltages from the analog pins on the feather M0
 *
 * @author Will Richards
 */
class Loom_Analog : public Module {
  public:
    void initialize() override;
    void measure() override;
    void display_data() override;
    void power_down() override {};
    void power_up() override {};

    /**
     * Templated constructor that only reads the battery voltage
     * @param man Reference to the manager
     */
    Loom_Analog(Manager &man);

    /**
     * Add a new pin to measure
     * @param pin The pin to get the data from eg. A0, A1, ...
     */
    void addMeasuredPin(uint8_t pin);

    /**
     * @param pin The pin to get the data from eg. A0, A1, ...
     */
    uint16_t getMv(uint8_t pin);

    /**
     * Get the analog ADC code from a given pin
     * @param pin The pin to get the data from eg. A0, A1, ...
     */
    uint16_t getAnalogCode(uint8_t pin);

  private:
    /* Convert the analog ADC code to mV */
    uint16_t analogToMV(uint16_t analogCode);

    /* Keep track of active analog pins */
    uint8_t activePins[MAX_ANALOG_PINS];

    /* Measured values for each monitored pin */
    struct AnalogMapping pinMappings[MAX_ANALOG_PINS];
};
