#include <Arduino.h>
#include <cstdint>

#include "Loom_Manager.h"
#include "Logger/Logger.h"
#include "Control/Loom_Interrupt/Loom_Interrupt.h"
#include "Control/Loom_RTC/Loom_RTC.h"
#include "Control/Loom_Sleep/Loom_Sleep.h"

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
        pause(100);
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

void Manager::logResetCause() {
    /* RCAUSE register sets flag to indicate cause of last reset.
     * See datasheet section 15.8.14 and
     * packages/arduino/tools/CMSIS-Atmel/1.2.0/CMSIS/Device/ATMEL/samd21/include/component/pm.h
     */
    uint8_t reset_cause = PM->RCAUSE.reg;

    if (reset_cause & PM_RCAUSE_POR) {
        LOG("Reset cause: Power-on");
    }
    if (reset_cause & PM_RCAUSE_BOD12) {
        LOG("Reset cause: 1.2V brown-out");
    }
    if (reset_cause & PM_RCAUSE_BOD33) {
        LOG("Reset cause: 3.3V brown-out");
    }
    if (reset_cause & PM_RCAUSE_EXT) {
        LOG("Reset cause: External (button)");
    }
    if (PM->RCAUSE.reg & PM_RCAUSE_WDT) {
        LOG("Reset cause: Watchdog timeout");
    }
    if (reset_cause & PM_RCAUSE_SYST) {
        LOG("Reset cause: System reset request");
    }

    if (reset_cause == 0) {
        ERROR("No reset cause listed");
    }
    /* Trick: check if multiple bits are set */
    if (reset_cause & (reset_cause - 1)) {
        WARNING("Multiple reset causes listed");
    }
}

// Initialize all modules
void Manager::initialize() {
    Loom_Interrupt::initialize();
    rails.initialize();
    sd.initialize();
    Logger::initialize(&sd, &rtcExternal);
    logResetCause();
    writeCsvHeader();

    LOG("*** Initializing ***");
    Loom_RTC::initialize();

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
    /* Power down peripherals */
    LOG("*** Powering Down ***");
    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->powerDown();
    }

    /* Force Serial reconnection when powering up */
    Serial.end();

    /* Since the SD card may have power disconnected, no logging
     * allowed until power is restored.  Device will hang. */
    rails.asleepMode();

    /* Turn off LED to indicate low power mode */
    digitalWrite(LED_BUILTIN, LOW);
}

void Manager::setAlarm(enum AlarmSource source, uint32_t millis) {
    switch (source) {

        case ALARM_RTC_INTERNAL:
            Loom_RTC::setAlarm(millis);
            break;

        case ALARM_RTC_EXTERNAL:
            rtcExternal.setAlarmIn(millis / 1000);
            break;
    }
}

void Manager::sleep() {
    /* Enter low-power consumption deep-sleep state.
     * Microcontroller will do nothing until the alarm triggers. */
    Loom_Sleep::sleep();
}

void Manager::powerUp(bool waitForSerial) {
    /* Turn on LED to indicate device awake */
    digitalWrite(LED_BUILTIN, HIGH);

    /* After this, SD power is restored and logging is allowed again. */
    rails.awakeMode();

    /* Allow time for Serial connection to establish with computer */
    uint64_t serialTimeout = waitForSerial ? 3000 : 0;
    beginSerial(serialTimeout);
    if (Serial) {
        LOG("Serial monitor reattached");
    }

    /* Power up peripherals */
    LOG("*** Powering Up ***");
    for (size_t i = 0; i < numRegisteredModules; i++) {
        modules[i]->powerUp();
    }
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
