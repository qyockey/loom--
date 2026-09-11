#include "Loom_TSL2591.h"
#include "Logger.h"

Loom_TSL2591::Loom_TSL2591(TSL2591Data *data, uint8_t addr) : Module() {
    this->data = data;
    this->addr = addr;
}

void Loom_TSL2591::initialize() {
    // Establish connection with sensor
    initialized = tsl.begin(addr);
    if (!initialized) {
        ERRORF("Module TSL2591 not detected at I2C address 0x%02X", addr);
        return;
    }

    // Set the gain and integration time of the sensor
    tsl.setGain(TSL2591_GAIN_MED);
    tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);
}

void Loom_TSL2591::measure() {
    if (!initialized) {
        return;
    }

    /* Pull the data from the sensor.  The low 16 bits are the full spectrum
     * measurement and the high 16 bits are infrared only.  Visible is the
     * difference between full spectrum and IR. */
    uint32_t fullLuminosity = tsl.getFullLuminosity();

    data->fullSpectrum = fullLuminosity & 0xFFFF;
    data->infrared = fullLuminosity >> 16;
    data->visible = data->fullSpectrum - data->infrared;
}

void Loom_TSL2591::displayData() {
    if (!initialized) {
        return;
    }

    Serial.printf("TSL2591:\n");
    Serial.printf("    Visible: %d\n", data->visible);
    Serial.printf("    Infrared: %d\n", data->infrared);
    Serial.printf("    Full Spectrum: %d\n", data->fullSpectrum);
    Serial.printf("\n");
}
