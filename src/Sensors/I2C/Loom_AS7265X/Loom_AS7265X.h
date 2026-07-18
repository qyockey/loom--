#pragma once

#include <SparkFun_AS7265X.h>
#include <Wire.h>

#include "../Module.h"
#include "Loom_Manager.h"

/**
 * AS7265X Full Spectrum Sensor
 *
 * @author Will Richards
 */
class Loom_AS7265X : public Module {
  protected:
    void power_down() override {};

    // Manager controlled functions
    void measure() override;
    void initialize() override;
    void package() override;
    void power_up() override;

  public:
    /**
     * Constructs a new AS7265X sensor
     * @param man Reference to the manager that is used to universally package all data
     * @param addr I2C address that is assigned to the sensor
     */
    Loom_AS7265X(Manager &man, int addr = 0x49);

  private:
    AS7265X asInst;   // Instance of the AS7265X

    uint16_t uv[6];    // Measured UV bands values. Units: counts / (μW/cm^2)
    uint16_t color[6]; // Measured color bands values. Units: counts / (μW/cm^2)
    uint16_t nir[6];   // Measured near-infra-red bands values. Units: counts / (μW/cm^2)
};
