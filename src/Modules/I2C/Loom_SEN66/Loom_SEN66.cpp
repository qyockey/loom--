#include <Wire.h>

#include "Hardware/Loom_RTC/Loom_RTC.h"
#include "Hardware/Loom_Sleep/Loom_Sleep.h"
#include "Logger.h"
#include "Loom_SEN66.h"

#define SEN66_SUCCESS 0

Loom_SEN66::Loom_SEN66(struct SEN66Data *data) : Module() {
    this->measured = data;

    fields[PM1].measured.u       = &data->pm1;
    fields[PM2p5].measured.u     = &data->pm2p5;
    fields[PM4].measured.u       = &data->pm4;
    fields[PM10].measured.u      = &data->pm10;
    fields[NUM_PM0p5].measured.u = &data->numPm0p5;
    fields[NUM_PM1].measured.u   = &data->numPm1;
    fields[NUM_PM2p5].measured.u = &data->numPm2p5;
    fields[NUM_PM4].measured.u   = &data->numPm4;
    fields[NUM_PM10].measured.u  = &data->numPm10;
    fields[HUMIDITY].measured.s  = &data->humidity;
    fields[TEMP_C].measured.s    = &data->temp_c;
    fields[VOC].measured.s       = &data->voc;
    fields[NOX].measured.s       = &data->nox;
    fields[CO2].measured.u       = &data->co2;
}

void Loom_SEN66::initialize() {
    Wire.begin();
    sen66.begin(Wire, SEN66_I2C_ADDR_6B);

    // Attempt to reset the device
    uint16_t status = sen66.deviceReset();

    if (status != SEN66_SUCCESS) {
        ERRORF("Reset failed with error code %u", status);
        return;
    }
    LOG("Sensor successfully reset!");

    LOG("Resetting SEN66, waiting 1.2s...");
    delay(1200);

    status = sen66.stopMeasurement();
    if (status != SEN66_SUCCESS) {
        ERRORF("Stop measurement failed with error code %u", status);
        return;
    }

    status = sen66.startContinuousMeasurement();
    if (status != SEN66_SUCCESS) {
        ERRORF("Continuous measurement failed with error code %u", status);
        return;
    }

    LOG("SEN66: Spinning up fan for 30 seconds");
    Loom_RTC::setAlarm(30 * 1000);
    Loom_Sleep::sleep();
    LOG("SEN66: Fan spin up complete");

    initialized = true;
}

bool Loom_SEN66::measureSingle() {
    uint16_t status;

    // Check if new data is ready
    uint8_t _padding;
    bool dataReady = false;
    status = sen66.getDataReady(_padding, dataReady);
    if (status != SEN66_SUCCESS) {
        ERRORF("Read error: %u", status);
        return false;
    }
    if (!dataReady) {
        WARNING("SEN66: Data not ready");
        return false;
    }

    status = sen66.readMeasuredValuesAsIntegers(
        fields[PM1].value.u,
        fields[PM2p5].value.u,
        fields[PM4].value.u,
        fields[PM10].value.u,
        fields[HUMIDITY].value.s,
        fields[TEMP_C].value.s,
        fields[VOC].value.s,
        fields[NOX].value.s,
        fields[CO2].value.u
    );

    if (status != SEN66_SUCCESS) {
        ERRORF("Read error: %u", status);
        return false;
    }

    status = sen66.readNumberConcentrationValuesAsIntegers(
        fields[NUM_PM0p5].value.u,
        fields[NUM_PM1].value.u,
        fields[NUM_PM2p5].value.u,
        fields[NUM_PM4].value.u,
        fields[NUM_PM10].value.u
    );

    if (status != SEN66_SUCCESS) {
        ERRORF("Read error: %u", status);
        return false;
    }

    return true;
}

void Loom_SEN66::accumulateReading() {
    for (uint8_t i = 0; i < FIELD_COUNT; i++) {
        SEN66_Field *field = &fields[i];

        if (field->isSigned) {
            if (field->value.s == INT16_MAX) {
                continue;
            }
            field->accumulator.s += field->value.s;
            ++field->count;
        }

        else {
            if (field->value.u == UINT16_MAX) {
                continue;
            }
            field->accumulator.u += field->value.u;
            ++field->count;
        }
    }
}

void Loom_SEN66::getAverageReading() {
    for (uint8_t i = 0; i < FIELD_COUNT; i++) {
        SEN66_Field *field = &fields[i];

        if (field->isSigned) {
            if (field->count == 0U) {
                *field->measured.s = INT16_MAX;
                WARNINGF("SEN66 %s data not valid", field->name);
                continue;
            }
            *field->measured.s = field->accumulator.s / field->count;
        }

        else {
            if (field->count == 0U) {
                *field->measured.u = UINT16_MAX;
                WARNINGF("SEN66 %s data not valid", field->name);
                continue;
            }
            *field->measured.u = field->accumulator.u / field->count;
        }
    }
}

void Loom_SEN66::clearReading() {
    for (uint8_t i = 0; i < FIELD_COUNT; i++) {
        SEN66_Field *field = &fields[i];

        /* Default to using error values */
        if (field->isSigned) {
            field->value.s = INT16_MAX;
        } else {
            field->value.u = UINT16_MAX;
        }
    }
}

void Loom_SEN66::measure() {
    if (!initialized) {
        return;
    }

    LOGF("SEN66: Measuring %d samples at 1 Hz", PM_AVERAGE_COUNT);
    for (uint8_t i = 0; i < PM_AVERAGE_COUNT; i++) {
        // Wait 1 second for next data point (Sensor updates @ 1Hz)
        delay(1000);

        clearReading();
        if (!measureSingle()) {
            continue;
        }
        accumulateReading();
    }
    getAverageReading();

    checkDeviceStatus();
}

void Loom_SEN66::displayData() {
    if (!initialized) {
        return;
    }

    Serial.print("SEN66:\n");
    for (uint8_t i = 0; i < FIELD_COUNT; i++) {
        SEN66_Field *field = &fields[i];
        Serial.printf("     %s (%s): ", field->name, field->unit);

        switch (i) {
            case VOC: case NOX:
                Serial.printf("%u\n", *field->measured.u / field->scale);
                break;
            case CO2:
                Serial.printf("%u\n", *field->measured.u / field->scale);
                break;
            default:
                Serial.printf(
                    "%0.2f\n",
                    (float) *field->measured.u / (float) field->scale
                );
                break;
        }
    }
    Serial.print("\n");
}

void Loom_SEN66::powerDown() {
    if (!initialized) {
        return;
    }

    int16_t status = sen66.stopMeasurement();
    if (status != SEN66_SUCCESS) {
        ERROR("SEN66: Error stopping continuous measurement");
        return;
    }
}

void Loom_SEN66::powerUp() {
    if (!initialized) {
        return;
    }

    int16_t status = sen66.startContinuousMeasurement();
    if (status != SEN66_SUCCESS) {
        ERROR("SEN66: Error starting continuous measurement");
        return;
    }

    LOG("SEN66: Spinning up fan for 30 seconds");
    Loom_RTC::setAlarm(30 * 1000);
    Loom_Sleep::sleep();
    LOG("SEN66: Fan spin up complete");
}

void Loom_SEN66::writeCsvHeader1(File *csv) {
    csv->print("SEN66,,,,,,,,,,,,,,");
}

void Loom_SEN66::writeCsvHeader2(File *csv) {
    for (uint8_t i = 0; i < FIELD_COUNT; i++) {
        SEN66_Field *field = &fields[i];
        csv->print(field->name);
        csv->print(" (");
        csv->print(field->unit);
        csv->print("),");
    }
}

void Loom_SEN66::writeCsvBody(File *csv) {
    if (!initialized) {
        csv->printf(",,,,,,,,,,,,,,");
        return;
    }

    for (uint8_t i = 0; i < FIELD_COUNT; i++) {
        SEN66_Field *field = &fields[i];
        switch (i) {
            case HUMIDITY: case TEMP_C: case VOC: case NOX:
                csv->printf("%3u,", *field->measured.s / field->scale);
                break;
            case CO2:
                csv->printf("%3u,", *field->measured.u / field->scale);
                break;
            default:
                csv->printf("%3.2f,", *field->measured.u / (float) field->scale);
                break;
        }
    }
}

void Loom_SEN66::checkDeviceStatus() {
    if (!initialized) {
        return;
    }

    SEN66DeviceStatus deviceStatus;
    uint16_t status = sen66.readDeviceStatus(deviceStatus);

    if (status != SEN66_SUCCESS) {
        ERRORF("Error reading device status: %u", status);
        return;
    }

    if (deviceStatus.value == 0) {
        // Nothing to worry about
        return;
    }

    WARNINGF(
        "SEN66 status: 0x%08lX.  See datasheet section 4.3 for more information",
        deviceStatus.value
    );

    if (deviceStatus.fanSpeedWarning) {
        WARNING("SEN66 Fan Speed Warning: Fan speed >10% away from target");
    }
    if (deviceStatus.co21Error) {
        ERROR("SEN66 CO2-1 Sensor Error: CO2 measurements may be wrong");
    }
    if (deviceStatus.pmError) {
        ERROR(
            "SEN66 Particulate Matter Sensor Error: "
            "PM measurements may be wrong"
        );
    }
    if (deviceStatus.co22Error) {
        ERROR("SEN66 CO2-2 Sensor Error: CO2 measurements may be wrong");
    }
    if (deviceStatus.gasError) {
        ERROR("SEN66 Gas Sensor Error: VOC and NOx measurements may be wrong");
    }
    if (deviceStatus.rhtError) {
        ERROR(
            "SEN66 Relative Humidity and Temperature Sensor Error: "
            "Temperature and humidity measurements may be wrong, and other "
            "values may be out of spec due to compensation"
        );
    }
    if (deviceStatus.fanError) {
        ERROR(
            "SEN66 Fan Error: "
            "Fan at 0 RPM, likely mechanically blocked or broken. "
            "All measured values are likely wrong."
        );
    }
}
