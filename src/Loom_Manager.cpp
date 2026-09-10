#include <Arduino.h>
#include <cstdint>

#include "Loom_Manager.h"
#include "Logger.h"


// Constructor
Manager::Manager(
    struct PacketData *packet,
    const char *devName,
    uint32_t instanceNum
) : deviceName(devName), instanceNumber(instanceNum) {
    numRegisteredModules = 0;
    this->packet = packet;
    readSerialNum();

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
}


// Append module to array
void Manager::registerModule(Module *moduleInst) {
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

    modules[numRegisteredModules] = moduleInst;
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
    digitalWrite(LED_BUILTIN, HIGH);
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
    ++packet->number;
    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->measure();
    }
}


// Display data from all modules
void Manager::displayData() {
    Serial.printf(
        "Packet:\n"
        "    Number: %lu\n"
        "\n",
        packet->number
    );

    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->displayData();
    }
}


void Manager::powerDown() {
    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->powerDown();
    }
}


void Manager::powerUp() {
    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->powerUp();
    }
}


void Manager::sleep(uint32_t millis, bool waitForSerial) {
    /* Allow time for message to get through before Serial bus loses power */
    LOG("Entering standby sleep");
    delay(50);

    Serial.end();

    /* Prepare peripherals for sleep and set power rails to sleep
     * configuration */
    powerDown();

    /* Enter low-power consumption deep-sleep state.
     * Microcontroller will do nothing until the alarm triggers. */
    digitalWrite(LED_BUILTIN, LOW);
    LowPower.sleep(millis);
    digitalWrite(LED_BUILTIN, HIGH);

    /* Wake peripherals from sleep and set power rails to awake
     * configuration. */
    powerUp();

    SLOG("Waking from sleep");

    /* Allow time for Serial connection to establish with computer */
    uint64_t serialTimeout = waitForSerial ? 3000 : 0;
    beginSerial(serialTimeout);
    LOG("Serial monitor reattached");
}


void Manager::readSerialNum() {
    /* Serial numbers are made up of four words located at these specific
     * registers (see datasheet section 9.3.3) */
    uint32_t snWords[4];
    snWords[0] = *(volatile uint32_t *)(0x0080A00C);
    snWords[1] = *(volatile uint32_t *)(0x0080A040);
    snWords[2] = *(volatile uint32_t *)(0x0080A044);
    snWords[3] = *(volatile uint32_t *)(0x0080A048);

    // Take these raw values and convert them into a string of hex characters
    ptrdiff_t wordOffset = 0;
    for (int i = 0; i < 4; i++) {
        snprintf(serialNum + wordOffset, 8 + 1, "%08lX", snWords[i]);
        wordOffset += 8;
    }
}


// Pause for specified time duration in ms
// NOTE: CPU runs at 100% during pause, use hypnos.sleep for low power
void Manager::pause(const uint32_t ms) const {
    uint32_t waitTime = millis() + ms;
    while (millis() < waitTime) {
    }
}
