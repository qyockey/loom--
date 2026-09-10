#pragma once

#include <Arduino.h>

#include "Module.h"

#define PIN_VBAT 7U
#define ADC_RESOLUTION_BITS 12
#define ADC_MAX_CODE ((1 << ADC_RESOLUTION_BITS) - 1)
#define ADC_VREF 3.3F

struct AnalogPinData {
    uint8_t number;
    uint16_t analogCode : ADC_RESOLUTION_BITS;
    uint16_t mv;
};

/**
 * Used to read analog voltages from the analog pins on the feather M0
 *
 * @author Will Richards
 */
class Loom_Analog : public Module {
  protected:
    void initialize() override;
    void measure() override;
    void display_data() override;
    void power_down() override {};
    void power_up() override {};

  public:
    /**
     * Reads voltage on an analog pin
     * @param pinData Pointer to analog pin measured data structure
     * @param pinNumber Number of analog pin to measure
     */
    Loom_Analog(struct AnalogPinData *pinData, uint8_t pinNumber);

  private:
    struct AnalogPinData *pin;

    /* Convert the analog ADC code to mV */
    uint16_t analogToMV(uint16_t analogCode);
};
