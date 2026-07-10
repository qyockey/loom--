#pragma once

#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2591.h>

#include "../I2CDevice.h"
#include "Loom_Manager.h"

/**
 * TSL2591 Light Sensor
 */
class Loom_TSL2591 : public Module {
  protected:
    // Manager controlled functions
    void measure() override;
    void initialize() override;
    void power_up() override;
    void power_down() override {};
    void display_data() override;

  public:
    /**
     * Constructs a new TSL2591 sensor
     * @param man Reference to the manager that is used to universally package all data
     */
    Loom_TSL2591(Manager &man);

  private:
    Adafruit_TSL2591 tsl; // Adafruit TSL2591 Sensor Object
    uint16_t lightLevels[3] = {0, 0, 0}; // Array of size 3 to hold all collected data
};
