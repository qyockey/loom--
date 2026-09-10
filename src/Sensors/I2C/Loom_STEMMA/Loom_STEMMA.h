#pragma once

#include <cstdint>
#include <Adafruit_seesaw.h>

#include "Module.h"

struct StemmaData {
    enum ModuleTag tag = MODULE_STEMMA;
    uint8_t temperatureC;
    uint16_t capacitive;
};

/**
 * STEMMA Soil Moisture sensor
 *
 * @author Will Richards
 */
class Loom_STEMMA : public Module {
  protected:
    void measure() override;
    void initialize() override;
    void display_data() override;
    void power_down() override {};
    void power_up() override {};

  public:
    /**
     * Constructs a new TSL2591 sensor
     * @param data Pointer to STEMMA measured data structure
     * @param addr I2C address that is assigned to the sensor
     */
    Loom_STEMMA(struct StemmaData *data, uint8_t address = 0x36);

  private:
    struct StemmaData *data;
    Adafruit_seesaw stemma; // Adafruit STEMMA Sensor Object

    uint8_t addr;
};
