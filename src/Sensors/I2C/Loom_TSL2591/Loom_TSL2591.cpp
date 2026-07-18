#include "Loom_TSL2591.h"

Loom_TSL2591::Loom_TSL2591(Manager &man) : Module() {
    man->registerModule(this);
}

void Loom_TSL2591::initialize() {
    // Establish connection with sensor
    tsl.begin();

    // Set the gain and integration time of the sensor
    power_up(this);
}

void Loom_TSL2591::measure() {
    // Pull the data from the sensor
    lightLevels[0] = tsl.getLuminosity(TSL2591_VISIBLE);;
    lightLevels[1] = tsl.getLuminosity(TSL2591_INFRARED);
    lightLevels[2] = tsl.getLuminosity(TSL2591_FULLSPECTRUM);
}

void Loom_TSL2591::display_data() {
    Serial.printf("TSL2591:\n");
    Serial.printf("    visible_counts: %d", lightLevels[0]);
    Serial.printf("    infrared_counts: %d", lightLevels[1]);
    Serial.printf("    full_spectrum_counts: %d", lightLevels[2]);
    Serial.printf("\n");
}

void Loom_TSL2591::power_up() {
    // Set the gain and integration time of the sensor
    tsl.setGain(TSL2591_GAIN_MED);
    tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);
}
