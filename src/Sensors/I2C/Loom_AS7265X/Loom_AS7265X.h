#pragma once

#include <SparkFun_AS7265X.h>

#include "Module.h"

struct AS7265XData {
    uint16_t uv[6];    // Measured UV bands values. Units: counts / (μW/cm^2)
    uint16_t color[6]; // Measured color bands values. Units: counts / (μW/cm^2)
    uint16_t nir[6];   // Measured near-infra-red bands values. Units: counts / (μW/cm^2)
};

/**
 * AS7265X Full Spectrum Sensor
 *
 * @author Will Richards
 */
class Loom_AS7265X : public Module {
  protected:
    void measure() override;
    void initialize() override;
    void displayData() override;
    void powerDown() override {};
    void powerUp() override {};

  public:
    /**
     * Constructs a new AS7265X sensor
     * @param data Pointer to AS7265X measured data structure
     * @param addr I2C address that is assigned to the sensor
     */
    Loom_AS7265X(struct AS7265XData *data, uint8_t addr = AS7265X_ADDR);

  private:
    struct AS7265XData *data;
    AS7265X asInst;    // Instance of the AS7265X
    uint8_t addr;
    bool initialized;
};
