#include "Loom_Hypnos.h"

Loom_Hypnos::Loom_Hypnos(Manager& man) : Module() {
    manInst = &man;

    // Set the pins to write mode
    pinMode(PIN_RAIL_3V, OUTPUT);
    pinMode(PIN_RAIL_5V, OUTPUT);

    // Add the Hypnos to the module register
    manInst->registerModule(this);
}

void Loom_Hypnos::initialize() {
    digitalWrite(PIN_RAIL_3V, RAIL_3V_ON);
    initializeRTC();
}

void Loom_Hypnos::display_data() {
    Serial.printf("Hypnos:\n");
    timeNowUtc = getCurrentTimeUtc();
    dateTime_toString(timeNowUtc, timeString);

    Serial.printf("    Time UTC: %s\n", timeString);
    Serial.printf("\n");
}

/* Power Rail Control Functionality */

void Loom_Hypnos::setPowerRails(PowerrailConfig railConfig) {
    /* See enum PowerrailConfig in Loom_Hypnos.h
     * railConfig = (3V << 1) | 5V
     * Extract 5V by taking only bit 0
     * Extract 3V by taking only bit 1 and shifting right
     *
     * This is a little silly but is justified because the prior implementation
     * wasted 60+ lines
     */
    uint8_t rail5vOn = railConfig & 0x01;
    uint8_t rail3vOn = (railConfig & 0x02) >> 1;
    digitalWrite(PIN_RAIL_3V, rail3vOn);
    digitalWrite(PIN_RAIL_5V, rail5vOn);

}
/* RTC */

void Loom_Hypnos::initializeRTC() {
    Serial.printf("Initializing RTC DS3231...\n");

    if (!RTC_DS.begin()) {
        Serial.printf("[ERROR] Couldn't start RTC!\n");
        return;
    }

    /* If RTC loses power, set time manually unless the unit is deployed in the
     * field with no serial interface */
    if (RTC_DS.lostPower() && Serial) {
        Serial.printf("RTC lost power, set the time\n");
        setCustomTime();
    }

    // Clear any pending alarms
    RTC_DS.clearAlarm();

    /* Configure INT/SQW output pin to give active-low interrupt instead of
     * square wave */
    RTC_DS.writeSqwPinMode(DS3231_OFF);

    Serial.printf("DS3231 real-time clock initialized successfully!\n");
    Serial.printf("UTC time now: ");
    dateTime_print(getCurrentTimeUtc());
    Serial.printf("\n");
}

DateTime Loom_Hypnos::getCurrentTimeUtc() {
    return RTC_DS.now();
}

void Loom_Hypnos::dateTime_toString(DateTime time, char *timeString) {
    // Formatted as: YYYY-MM-DDTHH:MM:SSZ
    /* Use modulus to limit number of digits strictly.  Otherwise compiler will
     * yell about 5 digit years and buffer overflows. */
    snprintf(timeString, 21, "%04u-%02u-%02uT%02u:%02u:%02uZ",
        (time.year() % 10000),
        (time.month() % 100),
        (time.day() % 100),
        (time.hour() % 100),
        (time.minute() % 100),
        (time.second() % 100)
    );
}

void Loom_Hypnos::dateTime_print(DateTime time) {
    // Formatted as: YYYY-MM-DDTHH:MM:SSZ
    Serial.printf("%04u-%02u-%02uT%02u:%02u:%02uZ",
        time.year(), time.month(), time.day(),
        time.hour(), time.minute(), time.second()
    );
}

int16_t readInt(const char *prompt, int16_t min, int16_t max) {
    while (true) {
        Serial.printf("\n%s (%d-%d)\n", prompt, min, max);

        // Block until at least one byte arrives in the Serial buffer
        while (Serial.available() == 0) {
            delay(10);
        }

        long value_long = Serial.parseInt();

        // Clear any leftover newline or carriage return characters from the buffer
        while (Serial.available() && (Serial.peek() == '\r' || Serial.peek() == '\n')) {
            Serial.read();
        }

        int16_t value_16 = (int16_t) value_long;
        if (value_long < INT16_MIN
            || value_long > INT16_MAX
            || value_16 < min
            || value_16 > max
        ) {
            Serial.printf("[WARNING] value %d out of range.\n", value_long);
            continue;
        }

        Serial.printf("Value entered: %d\n", value_16);
        return value_16;
    }
}

void Loom_Hypnos::setCustomTime() {
    Serial.printf(
        "\n"
        "###################################\n"
        "# Please use UTC time, not local! #\n"
        "###################################\n"
        "\n"
    );

    int16_t computer_year = readInt("Enter the year", 2000, 9999);
    int16_t computer_month = readInt("Enter the month", 1, 12);
    int16_t computer_day = readInt("Enter the day", 1, 31);
    int16_t computer_hour = readInt("Enter the hour", 0, 23);
    int16_t computer_minute = readInt("Enter the minute", 0, 59);
    int16_t computer_second = readInt("Enter the second", 0, 59);

    // Set the RTC to the custom time
    DateTime newTimeUtc = DateTime(
        computer_year, computer_month, computer_day,
        computer_hour, computer_minute, computer_second
    );
    RTC_DS.adjust(newTimeUtc);

    Serial.printf("Custom time successfully set to: %s");
    dateTime_print(getCurrentTimeUtc());
    Serial.printf("\n");
}

/* Sleep Functionality */

void Loom_Hypnos::sleep(TimeSpan duration) {
    /* Prepare modules for sleep */
    manInst->power_down();

    /* Safeguard: clear alarms if any have somehow activated */
    RTC_DS.clearAlarm();

    Serial.printf("Setting RTC alarm\n");
    DateTime timeAlarmUtc = RTC_DS.now() + duration;
    RTC_DS.setAlarm(timeAlarmUtc);
    Serial.printf("RTC alarm set for ");
    dateTimePrint(timeAlarmUtc);

    /* Set interrupt to monitor RTC alarm pin (#12).  This pin idles high then
     * is driven low by the RTC when it is time to wake up. */
    Serial.printf("Attaching RTC alarm interrupt\n");
    pinMode(PIN_RTC_ALARM, INPUT_PULLUP);
    /* Attaching twice, otherwise device won't wake up (not super sure why) */
    LowPower.attachInterruptWakeup(digitalPinToInterrupt(PIN_RTC_ALARM), wakeup, LOW);
    LowPower.attachInterruptWakeup(digitalPinToInterrupt(PIN_RTC_ALARM), wakeup, LOW);

    /* Allow time for message to get through before Serial bus loses power */
    Serial.printf("Entering standby sleep\n");
    delay(50);

    /* Cut power */
    Serial.end();
    USBDevice.detach();
    setPowerRails(railConfigAsleep);
    digitalWrite(LED_BUILTIN, LOW);

    /* Enter low-power consumption deep-sleep state.
     * Microcontroller will do nothing until the RTC alarm triggers. */
    LowPower.sleep();

    /* Restore power */
    digitalWrite(LED_BUILTIN, HIGH);
    setPowerRails(railConfigAwake);
    USBDevice.attach();
    manInst->beginSerial();

    /* Acknowledge RTC alarm.  The RTC will deassert its alarm so that the
     * interrupt pin returns high to an idle state. */
    RTC_DS.clearAlarm();

    /* Allow time for Serial connection to establish with computer */
    delay(1000);
    Serial.printf("Waking from sleep\n");

    /* Re-initialize all modules */
    manInst->power_up();
}

void Loom_Hypnos::wakeup() {
    /* Detach the interrupt immediately so it doesn't trigger again.
     * Otherwise this function gets called over and over in an infinite loop.
     * After returning, control flow moves to the line just after
     * the call to LowPower.sleep() where we start restoring power. */
    detachInterrupt(digitalPinToInterrupt(PIN_RTC_ALARM));
}
