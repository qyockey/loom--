#include <Arduino.h>
#include <cstdint>

#include "Loom_Manager.h"

// Constructor -- do nothing
Manager::Manager() {
}


// Append module to array
void Manager::registerModule(Module *module) {
    modules.push_back(module);
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


// Initialize all modules
void Manager::initialize() {
    for (size_t i = 0; i < modules.size(); i++) {
        modules[i]->initialize();
    }
}


// Measure data from all modules
void Manager::measure() {
    for (size_t i = 0; i < modules.size(); i++) {
        modules[i]->measure();
    }
}

// Display data from all modules
void Manager::display_data() {
    for (size_t i = 0; i < modules.size(); i++) {
        modules[i]->display_data();
    }
}


void Manager::power_down() {
    for (size_t i = 0; i < modules.size(); i++) {
        modules[i]->power_down();
    }
}


void Manager::power_up() {
    for (size_t i = 0; i < modules.size(); i++) {
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
