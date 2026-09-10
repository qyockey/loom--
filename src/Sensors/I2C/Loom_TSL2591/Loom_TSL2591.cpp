#include "Loom_TSL2591.h"

Loom_TSL2591::Loom_TSL2591(TSL2591Data *data) : Module() {
    this->data = data;
}

void Loom_TSL2591::initialize() {
    // Establish connection with sensor
    tsl.begin();

    // Set the gain and integration time of the sensor
    tsl.setGain(TSL2591_GAIN_MED);
    tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);
}

void Loom_TSL2591::measure() {
    /* Pull the data from the sensor.  The low 16 bits are the full spectrum
     * measurement and the high 16 bits are infrared only.  Visible is the
     * difference between full spectrum and IR. */
    uint32_t fullLuminosity = tsl.getFullLuminosity();

    data->fullSpectrum = fullLuminosity & 0xFFFF;
    data->infrared = fullLuminosity >> 16;
    data->visible = data->fullSpectrum - data->infrared;
}

void Loom_TSL2591::display_data() {
    Serial.printf("TSL2591:\n");
    Serial.printf("    visible_counts: %d\n", data->visible);
    Serial.printf("    infrared_counts: %d\n", data->infrared);
    Serial.printf("    full_spectrum_counts: %d\n", data->fullSpectrum);
    Serial.printf("\n");
}
