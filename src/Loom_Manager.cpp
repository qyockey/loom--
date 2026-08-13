#include <Arduino.h>
#include <cstdint>

#include "Loom_Manager.h"

// Constructor
Manager::Manager() {
    numRegisteredModules = 0;
}


// Append module to array
void Manager::registerModule(Module *module) {
    if (LOOM_MAX_MODULES <= numRegisteredModules) {
        Serial.printf(
            "[ERROR] Maximum number of modules exceeded!\n"
            "        Increase defined value of LOOM_MAX_MODULES (currently %u)\n"
            "        For example, add this line at the top of your sketch\n"
            "\n"
            "        #define LOOM_MAX_MODULES 32\n"
            "\n",
            LOOM_MAX_MODULES
        );
        return;
    }

    modules[numRegisteredModules] = module;
    numRegisteredModules++;
}


// Initialize Serial interface
void Manager::beginSerial() {
    long startMillis = millis();

    Serial.begin(BAUD_RATE);
    while (!Serial) {

        // Abort if time elapsed exceeds acceptable duration
        if (millis() >= (startMillis + LOOM_SERIAL_WAIT_MS)) {
            break;
        }

        // Flash LED as visual indicator of waiting
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        delay(100);
    }
}


// Measure data from all modules
void Manager::measure() {
    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->measure();
    }
}


// Display data from all modules
void Manager::display_data() {
    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->display_data();
    }
}


// Initialize all modules
void Manager::initialize() {
    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->initialize();
    }
}


void Manager::power_down() {
    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->power_down();
    }
}


void Manager::power_up() {
    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->power_up();
    }
}


// Pause for specified time duration in ms
// NOTE: CPU runs at 100% during pause, use hypnos.sleep for low power
void Manager::pause(const uint32_t ms) const {
    uint32_t waitTime = millis() + ms;
    while (millis() < waitTime) {
    }
}
