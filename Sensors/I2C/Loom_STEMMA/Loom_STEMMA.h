#pragma once

#include <Adafruit_seesaw.h>

#include "Loom_Manager.h"

/**
 * STEMMA Soil Moisture sensor
 *
 * @author Will Richards
 */
class Loom_STEMMA : public Module {
  protected:
    // Manager controlled functions
    void measure() override;
    void initialize() override;
    void power_up() override {};
    void power_down() override {};

  public:
    /**
     * Constructs a new TSL2591 sensor
     * @param man Reference to the manager that is used to universally package all data
     * @param address I2C address that is assigned to the sensor
     */
    Loom_STEMMA(Manager &man, int addr = 0x36);

  private:
    Adafruit_seesaw stemma; // Adafruit STEMMA Sensor Object
    int address;

    float temperature; // Soil temperature
    uint16_t cap;      // Soil capacitive
};
