#include <Arduino.h>
#include <ArduinoLowPower.h>
#include <cstdint>

#include "Loom_Manager.h"
#include "Logger.h"

// Constructor
Manager::Manager(
    struct PacketMetadata *metadata,
    const char *devName,
    uint32_t instanceNum,
    HypnosVersion hypnosVersion
) :
    deviceName(devName),
    instanceNumber(instanceNum),
    packetNumber(&metadata->packetNumber),
    rtcExternal(&metadata->timestamp),
    sd(hypnosVersion, deviceName) {

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    numRegisteredModules = 0;
    this->packetNumber = packetNumber;

    registerModule(&packetNumber);
    registerModule(&rtcExternal);
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

void Manager::setWakeConfiguration(struct PowerRailConfig config) {
    if (config.rail_3v == RAIL_3V_OFF) {
        WARNING(
            "Disabling 3V rail while awake is likely a mistake.  "
            "Because the 3V rail controls the I2C and SPI communication lines, "
            "attempting to communicate on these buses will result in an  "
            "invalid state that will cause the processor to hang."
        );
    }
    rails.setWakeConfiguration(config);
}

/**
 * Set the configuration for the power rails when going to sleep
 * @param config The desired configuration while the device is asleep
 * See namespace RailState in Loom_PowerRail.h.
 */
void Manager::setSleepConfiguration(struct PowerRailConfig config) {
    rails.setSleepConfiguration(config);
}

// Initialize all modules
void Manager::initialize() {
    rails.initialize();

    sd.initialize();
    Logger::initialize(&sd, &rtcExternal);
    writeCsvHeader();

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
void Manager::displayData() {
    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->displayData();
    }
}

void Manager::writeCsvHeader() {
    LOG("Writing header to CSV file");
    File *csvFile = sd.getCsvFile();
    if (csvFile == nullptr) {
        return;
    }

    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->writeCsvHeader1(csvFile);
    }
    csvFile->printf("\n");

    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->writeCsvHeader2(csvFile);
    }
    csvFile->printf("\n");

    csvFile->close();
}

void Manager::logToSd() {
    LOG("Writing data to CSV file");
    File *csvFile = sd.getCsvFile();
    if (csvFile == nullptr) {
        return;
    }

    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->writeCsvBody(csvFile);
    }
    csvFile->printf("\n");

    csvFile->close();
}

void Manager::powerDown() {
    SLOG("*** Powering Down ***");
    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->powerDown();
    }
}

void Manager::powerUp() {
    SLOG("*** Powering Up ***");
    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->powerUp();
    }
}

void Manager::sleep(uint32_t millis, bool waitForSerial) {
    /* Prepare peripherals for sleep */
    powerDown();

    /* Allow time for message to get through before SD SPI bus loses power */
    LOG("Entering standby sleep");
    delay(50);
    Serial.end();

    /* Since the SD card may have power disconnected, no logging
     * allowed until power is restored.  Device will hang. */
    rails.asleepMode();

    /* Enter low-power consumption deep-sleep state.
     * Microcontroller will do nothing until the alarm triggers. */
    digitalWrite(LED_BUILTIN, LOW);
    LowPower.sleep(millis);
    digitalWrite(LED_BUILTIN, HIGH);

    /* After this, SD power is restored and logging is allowed again. */
    rails.awakeMode();

    /* We can log to SD card but Serial interface isn't ready yet */
    SLOG("Waking from sleep");

    /* Allow time for Serial connection to establish with computer */
    uint64_t serialTimeout = waitForSerial ? 3000 : 0;
    beginSerial(serialTimeout);
    LOG("Serial monitor reattached");

    /* Wake peripherals from sleep */
    powerUp();
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
    for (int i = 0; i < 4; i++) {
        snprintf(serialNum + 8 * i, 8 + 1, "%08lX", snWords[i]);
    }
}

// Pause for specified time duration in ms
// NOTE: CPU runs at 100% during pause, use manager.sleep for low power
void Manager::pause(const uint32_t ms) const {
    uint32_t waitTime = millis() + ms;
    while (millis() < waitTime) {
    }
}
