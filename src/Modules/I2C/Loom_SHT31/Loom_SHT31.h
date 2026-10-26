#pragma once

#include <Adafruit_SHT31.h>

#include "Modules/Module.h"

#define LOOM_SHT31_ADDR_DEFAULT 0x44

struct SHT31Data {
    float temperatureC;
    float humidity;
};

/**
 *  SHT31 Temperature and Humidity Sensor
 * 
 *  @author Will Richards
 */ 
class Loom_SHT31 : public Module {
  protected:
    void initialize() override;
    void measure() override;
    void displayData() override;
    void powerDown() override {};
    void powerUp() override {};
    void writeCsvHeader1(File *csv) override;
    void writeCsvHeader2(File *csv) override;
    void writeCsvBody(File *csv) override;

  public:
    /**
     * Constructs a new SHT31 sensor
     * @param data Pointer to SHT31 measured data structure
     * @param addr I2C address that is assigned to the sensor
     * @param muxPort I²C port number if connected via TCA9548A multiplexer,
     *        -1 if not connected via multiplexer
     */ 
    Loom_SHT31(
        struct SHT31Data *data,
        uint8_t address = LOOM_SHT31_ADDR_DEFAULT,
        int8_t muxPort = -1
    );

  private:
    struct SHT31Data *data;
    Adafruit_SHT31 sht;

    uint8_t addr;
    bool initialized = false;
};
