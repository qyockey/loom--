#include "Loom_TSL2591.h"

Loom_TSL2591::Loom_TSL2591(Manager &man) : Module() {
    manInst = &man;
    manInst->registerModule(this);
}

void Loom_TSL2591::initialize() {
    // Establish connection with sensor
    tsl.begin();

    // Set the gain and integration time of the sensor
    tsl.setGain(TSL2591_GAIN_MED);
    tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);
}

void Loom_TSL2591::measure() {
    // Pull the data from the sensor
    lightLevels[0] = tsl.getLuminosity(TSL2591_VISIBLE);;
    lightLevels[1] = tsl.getLuminosity(TSL2591_INFRARED);
    lightLevels[2] = tsl.getLuminosity(TSL2591_FULLSPECTRUM);
}

void Loom_TSL2591::display_data() {
    Serial.printf("TSL2591:\n");
    Serial.printf("    visible_counts: %d\n", lightLevels[0]);
    Serial.printf("    infrared_counts: %d\n", lightLevels[1]);
    Serial.printf("    full_spectrum_counts: %d\n", lightLevels[2]);
    Serial.printf("\n");
}
