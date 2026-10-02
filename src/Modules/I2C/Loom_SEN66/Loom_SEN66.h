#pragma once

#include <SensirionI2cSen66.h>

#include "Modules/Module.h"
#include "Loom_Manager.h"

#define PM_AVERAGE_COUNT 10    // Number of times to read the pm values then average them over
#define SEN66_I2C_ADDRESS 0x6B // Standard I2C address for SEN66

struct SEN66Data {
    uint16_t pm1;      // 0.1 μg/m³
    uint16_t pm2p5;    // 0.1 μg/m³
    uint16_t pm4;      // 0.1 μg/m³
    uint16_t pm10;     // 0.1 μg/m³
    uint16_t numPm0p5; // 0.1 particles/cm³ 
    uint16_t numPm1;   // 0.1 particles/cm³ 
    uint16_t numPm2p5; // 0.1 particles/cm³ 
    uint16_t numPm4;   // 0.1 particles/cm³ 
    uint16_t numPm10;  // 0.1 particles/cm³ 
    int16_t humidity;  // 0.01 %RH
    int16_t temp_c;    // 0.005 °C
    int16_t voc;       // 0.1 index
    int16_t nox;       // 0.1 index
    uint16_t co2;      // 1.0 ppm
};

/**
 * SEN66 Air Quality Sensor
 * Supports: PM 1.0, 2.5, 4.0, 10.0, Humidity, Temperature, VOC Index,
 * NOx Index, and CO2.
 *
 * NOTE: The SEN66 reads PM, Gas, and CO2 in a single block.
 *
 * @author Soren Emmons, Quinn Yockey
 */
class Loom_SEN66 : public Module {
  protected:
    void initialize() override;
    void measure() override;
    void displayData() override;
    void powerDown() override;
    void powerUp() override;
    void writeCsvHeader1(File *csv) override;
    void writeCsvHeader2(File *csv) override;
    void writeCsvBody(File *csv) override;

  public:
    /**
     */
    Loom_SEN66(SEN66Data *data);

  private:
    bool measureSingle();
    void accumulateReading();
    void getAverageReading();
    void checkDeviceStatus();
    void getScaledAverageReading();
    void clearReading();

    SensirionI2cSen66 sen66; // Instance of the SEN66 driver object
    SEN66Data *measured;
    bool initialized = false;

    struct SEN66_Field {
        const char *name;
        const char *unit;
        bool isSigned;
        union {
            uint16_t u;
            int16_t s;
        } value;
        union {
            uint32_t u;
            int32_t s;
        } accumulator;
        union {
            uint16_t *u;
            int16_t *s;
        } measured;
        uint8_t count;
        uint8_t scale;
    };

    enum Field {
        PM1, PM2p5, PM4, PM10,
        NUM_PM0p5, NUM_PM1, NUM_PM2p5, NUM_PM4, NUM_PM10,
        HUMIDITY, TEMP_C, VOC, NOX, CO2,
        FIELD_COUNT,
    };

    /* Scaling factors and units listed in datasheet 4.8.7 and 4.8.13 */
    struct SEN66_Field fields[FIELD_COUNT] = {
        {
            .name = "PM1",
            .unit = "μg/m³",
            .isSigned = false,
            .value = { .u = 0U },
            .accumulator = { .u = 0UL },
            .measured = { .u = nullptr },
            .count = 0U,
            .scale = 10U,
        },
        {
            .name = "PM2.5",
            .unit = "μg/m³",
            .isSigned = false,
            .value = { .u = 0U },
            .accumulator = { .u = 0UL },
            .measured = { .u = nullptr },
            .count = 0U,
            .scale = 10U,
        },
        {
            .name = "PM4",
            .unit = "μg/m³",
            .isSigned = false,
            .value = { .u = 0U },
            .accumulator = { .u = 0UL },
            .measured = { .u = nullptr },
            .count = 0U,
            .scale = 10U,
        },
        {
            .name = "PM10",
            .unit = "μg/m³",
            .isSigned = false,
            .value = { .u = 0U },
            .accumulator = { .u = 0UL },
            .measured = { .u = nullptr },
            .count = 0U,
            .scale = 10U,
        },
        {
            .name = "Number PM0.5",
            .unit = "μg/m³",
            .isSigned = false,
            .value = { .u = 0U },
            .accumulator = { .u = 0UL },
            .measured = { .u = nullptr },
            .count = 0U,
            .scale = 10U,
        },
        {
            .name = "Number PM1",
            .unit = "particles/cm³",
            .isSigned = false,
            .value = { .u = 0U },
            .accumulator = { .u = 0UL },
            .measured = { .u = nullptr },
            .count = 0U,
            .scale = 10U,
        },
        {
            .name = "Number PM2.5",
            .unit = "particles/cm³",
            .isSigned = false,
            .value = { .u = 0U },
            .accumulator = { .u = 0UL },
            .measured = { .u = nullptr },
            .count = 0U,
            .scale = 10U,
        },
        {
            .name = "Number PM4",
            .unit = "particles/cm³",
            .isSigned = false,
            .value = { .u = 0U },
            .accumulator = { .u = 0UL },
            .measured = { .u = nullptr },
            .count = 0U,
            .scale = 10U,
        },
        {
            .name = "Number PM10",
            .unit = "particles/cm³",
            .isSigned = false,
            .value = { .u = 0U },
            .accumulator = { .u = 0UL },
            .measured = { .u = nullptr },
            .count = 0U,
            .scale = 10U,
        },
        {
            .name = "Humidity",
            .unit = "%RH",
            .isSigned = true,
            .value = { .s = 0U },
            .accumulator = { .s = 0UL },
            .measured = { .s = nullptr },
            .count = 0U,
            .scale = 100U,
        },
        {
            .name = "Temperature",
            .unit = "°C",
            .isSigned = true,
            .value = { .s = 0U },
            .accumulator = { .s = 0UL },
            .measured = { .s = nullptr },
            .count = 0U,
            .scale = 200U,
        },
        {
            .name = "VOC",
            .unit = "0-500",
            .isSigned = true,
            .value = { .s = 0U },
            .accumulator = { .s = 0UL },
            .measured = { .s = nullptr },
            .count = 0U,
            .scale = 10U,
        },
        {
            .name = "NOx",
            .unit = "0-500",
            .isSigned = true,
            .value = { .s = 0U },
            .accumulator = { .s = 0UL },
            .measured = { .u = nullptr },
            .count = 0U,
            .scale = 10U,
        },
        {
            .name = "CO₂",
            .unit = "ppm",
            .isSigned = false,
            .value = { .u = 0U },
            .accumulator = { .u = 0UL },
            .measured = { .u = nullptr },
            .count = 0U,
            .scale = 1U,
        },
    };
};
