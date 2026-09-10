#pragma once

#include <Arduino.h>

#include "Module.h"

struct DigitalPinData {
    uint8_t number;
    bool state;
};

/**
 * Used to read digital state voltages from a given pin on the feather M0
 */
class Loom_Digital : public Module {
  protected:
    void initialize() override {};
    void measure() override;
    void display_data() override;
    void power_down() override {};
    void power_up() override {};

  public:
    /**
     * Reads state of a digital pin
     * @param pinData Pointer to digital pin measured data structure
     * @param pinNumber Number of digital pin to measure
     */
    Loom_Digital(struct DigitalPinData *pinData, uint8_t pinNumber);

  private:
    struct DigitalPinData *pin;
};
