#include <Arduino.h>
#include <cstdint>

#include "Loom_Manager.h"
#include "Logger.h"

// Constructor
Manager::Manager(const char *devName, uint32_t instanceNum)
    : deviceName(devName), instanceNumber(instanceNum) {
    numRegisteredModules = 0;
    read_serial_num();
}


// Append module to array
void Manager::registerModule(Module *module) {
    if (LOOM_MAX_MODULES <= numRegisteredModules) {
        ERRORF(
            "Maximum number of modules exceeded!\n"
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
void Manager::beginSerial(uint64_t timeoutMillis) {
    Serial.begin(BAUD_RATE);

    uint64_t startMillis = millis();
    while (!Serial && millis() - startMillis < timeoutMillis) {
        // Flash LED as visual indicator of waiting
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        delay(100);
    }
}


// Initialize all modules
void Manager::initialize() {
    LOG("*** Initializing ***");
    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->initialize();
    }
}


// Measure data from all modules
void Manager::measure() {
    LOG("*** Measuring ***");
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


void Manager::power_down() {
    LOG("*** Powering Down ***");
    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->power_down();
    }
}


void Manager::power_up() {
    LOG("*** Powering Up ***");
    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->power_up();
    }
}


void Manager::read_serial_num() {
    /* Serial numbers are made up of four words located at these specific
     * registers (see datasheet section 9.3.3) */
    uint32_t sn_words[4];
    sn_words[0] = *(volatile uint32_t *)(0x0080A00C);
    sn_words[1] = *(volatile uint32_t *)(0x0080A040);
    sn_words[2] = *(volatile uint32_t *)(0x0080A044);
    sn_words[3] = *(volatile uint32_t *)(0x0080A048);

    // Take these raw values and convert them into a string of hex characters
    ptrdiff_t word_offset = 0;
    for (int i = 0; i < 4; i++) {
        snprintf(serial_num + word_offset, 8 + 1, "%08lX", sn_words[i]);
        word_offset += 8;
    }
}


// Pause for specified time duration in ms
// NOTE: CPU runs at 100% during pause, use hypnos.sleep for low power
void Manager::pause(const uint32_t ms) const {
    uint32_t waitTime = millis() + ms;
    while (millis() < waitTime) {
    }
}
