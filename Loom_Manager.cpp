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
    Serial.begin(BAUD_RATE);
    while (!Serial) {
    }
}


// Measure data from all modules
void Manager::measure() {
    for (int i = 0; i < modules.size(); i++) {
        modules[i]->measure();
    }
}

// Power up all modules
void Manager::power_up() {
    for (int i = 0; i < modules.size(); i++) {
        modules[i]->power_up();
    }
}


// Power down all modules
void Manager::power_down() {
    for (int i = 0; i < modules.size(); i++) {
        modules[i]->power_down();
    }
}


// Display data from all modules
void Manager::display_data() {
    for (int i = 0; i < modules.size(); i++) {
        modules[i]->display_data();
    }
}


// Initialize all modules
void Manager::initialize() {
    for (int i = 0; i < modules.size(); i++) {
        modules[i]->initialize();
    }
}


// Pause for specified time duration in ms
// NOTE: CPU runs at 100% during pause, use hypnos.sleep for low power
void Manager::pause(const uint32_t ms) const {
    int waitTime = millis() + ms;
    while (millis() < waitTime) {
    }
}
