#pragma once

#include <cstdint>

#include "../../../Loom_Manager.h"
#include "../../../Module.h"
#include "Adafruit_seesaw.h"

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
    void display_data() override;

  public:
    /**
     * Constructs a new TSL2591 sensor
     * @param man Reference to the manager that is used to universally package all data
     * @param address I2C address that is assigned to the sensor
     */
    Loom_STEMMA(Manager &man, uint8_t address);

  private:
    Manager *manInst;
    Adafruit_seesaw stemma; // Adafruit STEMMA Sensor Object

    uint8_t address;
    float temperature; // Soil temperature
    uint16_t cap;      // Soil capacitive
};
