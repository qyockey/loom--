#pragma once

#include <Adafruit_TSL2591.h>

#include "Module.h"

struct TSL2591Data {
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
    void displayData() override;
    void powerDown() override {};
    void powerUp() override {};

  public:
    /**
     * Constructs a new TSL2591 sensor
     * @param data Pointer to TSL2591 measured data structure
     * @param addr I2C address
     */
    Loom_TSL2591(TSL2591Data *data, uint8_t addr = TSL2591_ADDR);

  private:
    Adafruit_TSL2591 tsl; // Adafruit TSL2591 Sensor Object
    TSL2591Data *data;
    uint8_t addr;
    bool initialized;
};
