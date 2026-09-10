#pragma once

#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2591.h>

#include "Module.h"

struct TSL2591Data {
    enum ModuleTag tag = MODULE_TSL2591;
    uint16_t visible;
    uint16_t infrared;
    uint16_t fullSpectrum;
};

/**
 * TSL2591 Light Sensor
 */
class Loom_TSL2591 : public Module {
  protected:
    void measure() override;
    void initialize() override;
    void display_data() override;
    void power_down() override {};
    void power_up() override {};

  public:
    /**
     * Constructs a new TSL2591 sensor
     * @param data Pointer to TSL2591 measured data structure
     */
    Loom_TSL2591(TSL2591Data *data);

  private:
    Adafruit_TSL2591 tsl; // Adafruit TSL2591 Sensor Object
    TSL2591Data *data;
};
