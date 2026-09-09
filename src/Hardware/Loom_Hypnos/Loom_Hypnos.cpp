#include <Arduino.h>

#include "Loom_Hypnos.h"
#include "Logger.h"

volatile bool Loom_Hypnos::shouldPowerUp = false;

Loom_Hypnos::Loom_Hypnos(Manager& man, HypnosVersion version) : Module() {
    manInst = &man;

    /* Create SD card manager */
    sdChipSelect = (uint8_t) version;
    sdMan = new SdManager(sdChipSelect, man.get_device_name());

    manInst->registerModule(this);
}

void Loom_Hypnos::initialize() {
    /* Set the rail pins to output mode */
    pinMode(PIN_RAIL_3V, OUTPUT);
    pinMode(PIN_RAIL_5V, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    /* Monitor RTC alarm pin using input configured with internal pull-up
     * resistor enabled */
    pinMode(PIN_RTC_ALARM, INPUT_PULLUP);

    setPowerRails(railConfigAwake);
    initializeRtc();
    sdMan->initialize();
    Logger::initialize(sdMan, this);
}

void Loom_Hypnos::display_data() {
    Serial.printf(
        "Hypnos:\n"
        "    Time UTC: %s\n",
        RTC_DS.now().text()
    );
}

/* Power Rail Control Functionality */

void Loom_Hypnos::setPowerRails(struct PowerrailConfig railConfig) {
    digitalWrite(PIN_RAIL_3V, railConfig.rail_3v);
    digitalWrite(PIN_RAIL_5V, railConfig.rail_5v);
}

/* RTC */

void Loom_Hypnos::initializeRtc() {
    LOG("Initializing RTC DS3231...");

    if (!RTC_DS.begin()) {
        ERROR("Couldn't start RTC!");
        return;
    }

    /* If RTC loses power, set time manually unless the unit is deployed in the
     * field with no serial interface */
    if (RTC_DS.lostPower() && Serial) {
        LOG("RTC lost power, set the time");
        setCustomTime();
    }

    /* Clear any pending alarms */
    RTC_DS.clearAlarm();

    /* Configure INT/SQW output pin to give active-low interrupt instead of
     * square wave */
    RTC_DS.writeSqwPinMode(DS3231_OFF);

    LOG("DS3231 real-time clock initialized successfully!");
    LOGF("UTC time now: %s", RTC_DS.now().text());
}

DateTime Loom_Hypnos::getCurrentTimeUtc() {
    return RTC_DS.now();
}

int16_t Loom_Hypnos::serialReadInt(const char *prompt, int16_t min, int16_t max) {
    while (true) {
        Serial.printf("%s (%d-%d)\n", prompt, min, max);

        /* Block until at least one byte arrives in the Serial buffer */
        while (Serial.available() == 0) {
            delay(10);
        }

        long value_long = Serial.parseInt();

        /* Clear any leftover newline or carriage return characters from the
         * buffer */
        while (Serial.available()) {
            char next = Serial.peek();
            if (next != '\r' && next != '\n') {
                break;
            }
            Serial.read();
        }

        int16_t value_16 = (int16_t) value_long;
        if (value_long < INT16_MIN
            || value_long > INT16_MAX
            || value_16 < min
            || value_16 > max
        ) {
            Serial.printf("[WARNING] value %ld out of range.\n", value_long);
            continue;
        }

        Serial.printf("Value entered: %d\n", value_16);
        return value_16;
    }
}

void Loom_Hypnos::setCustomTime() {
    /* Print call is broken up, otherwise last line is mysteriously truncated */
    LOGF(
        "\n"
        "############################\n"
        "# Use UTC time, not local! #\n"
        "############################\n"
        "\n"
    );

    int16_t computer_year = serialReadInt("Enter the year", 2000, 9999);
    int16_t computer_month = serialReadInt("Enter the month", 1, 12);
    int16_t computer_day = serialReadInt("Enter the day", 1, 31);
    int16_t computer_hour = serialReadInt("Enter the hour", 0, 23);
    int16_t computer_minute = serialReadInt("Enter the minute", 0, 59);
    int16_t computer_second = serialReadInt("Enter the second", 0, 59);

    DateTime newTimeUtc = DateTime(
        computer_year, computer_month, computer_day,
        computer_hour, computer_minute, computer_second
    );
    RTC_DS.adjust(newTimeUtc);

    LOGF("Custom time successfully set to %s", RTC_DS.now().text());
}

/* Sleep Functionality */

void Loom_Hypnos::sleep(TimeSpan duration) {
    /* Prepare modules for sleep */
    manInst->power_down();

    /* Safeguard: clear alarms if any have somehow activated */
    RTC_DS.clearAlarm();

    LOG("Setting RTC alarm");
    DateTime timeAlarmUtc = RTC_DS.now() + duration;
    RTC_DS.setAlarm(timeAlarmUtc);
    LOGF("RTC alarm set for %s", timeAlarmUtc.text());

    /* Safeguard: clear interrupt pending flag before enabling interrupt */
    EIC->INTFLAG.bit.EXTINT3 = 1;

    /* Set interrupt to monitor RTC alarm pin (#12). */
    LOG("Attaching RTC alarm interrupt");
    LowPower.attachInterruptWakeup(PIN_RTC_ALARM, wakeup, LOW);

    /* Allow time for message to get through before Serial bus loses power */
    LOG("Entering standby sleep");
    delay(50);

    /* Cut power to peripherals */
    Serial.end();
    USBDevice.detach();
    setPowerRails(railConfigAsleep);
    digitalWrite(LED_BUILTIN, LOW);

    /* Enter low-power consumption deep-sleep state.
     * Microcontroller will do nothing until the RTC alarm triggers.
     * Use boolean shouldPowerUp to differentieate RTC alarm from any other
     * interrupt. */
    shouldPowerUp = false;
    do {
        LowPower.sleep();
    } while (shouldPowerUp == false);

    /* Restore power */
    digitalWrite(LED_BUILTIN, HIGH);
    setPowerRails(railConfigAwake);
    USBDevice.attach();

    /* Acknowledge RTC alarm.  The RTC will deassert its alarm so that the
     * interrupt pin returns high to an idle state. */
    RTC_DS.clearAlarm();

    /* Explicitly clear interrupt pending flag after RTC alarm is cleared */
    EIC->INTFLAG.bit.EXTINT3 = 1;

    /* Allow time for Serial connection to establish with computer */
    manInst->beginSerial(1000);
    LOG("Waking from sleep");

    /* Re-initialize all modules */
    manInst->power_up();
}

void Loom_Hypnos::wakeup() {
    /* Detach the interrupt immediately so it doesn't trigger again.
     * Otherwise the interrupt service routine gets called over and over in an
     * infinite loop. */
    detachInterrupt(PIN_RTC_ALARM);

    /* Use boolean shouldPowerUp to record that the interrupt received was
     * indeed the RTC alarm. After returning, control flow moves to the line
     * just after the call to LowPower.sleep(). */
    shouldPowerUp = true;
}
