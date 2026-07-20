#pragma once

#include <cstdint>
#include <Wire.h>

#include "../../../Module.h"
#include "../../../Loom_Manager.h"
#include "SparkFun_AS7265X.h"

/**
 * AS7265X Full Spectrum Sensor
 *
 * @author Will Richards
 */
class Loom_AS7265X : public Module {
  protected:
    // Manager controlled functions
    void measure() override;
    void initialize() override;
    void display_data() override;

  public:
    /**
     * Constructs a new AS7265X sensor
     * @param man Reference to the manager that is used to universally package all data
     * @param addr I2C address that is assigned to the sensor
     */
    Loom_AS7265X(Manager &man, uint8_t addr = 0x49);

  private:
    Manager *manInst;
    AS7265X asInst;    // Instance of the AS7265X
    uint8_t address;

    uint16_t uv[6];    // Measured UV bands values. Units: counts / (μW/cm^2)
    uint16_t color[6]; // Measured color bands values. Units: counts / (μW/cm^2)
    uint16_t nir[6];   // Measured near-infra-red bands values. Units: counts / (μW/cm^2)
};
