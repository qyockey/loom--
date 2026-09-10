#include <Arduino.h>

#include "Loom_Hypnos.h"
#include "Logger.h"

Loom_Hypnos::Loom_Hypnos(
    struct TimestampData *timestamp,
    HypnosVersion version,
    const char *deviceName
) : Module() {
    this->timestamp = timestamp;

    /* Create SD card manager */
    sdChipSelect = (uint8_t) version;
    sdMan = new SdManager(sdChipSelect, deviceName);
}

void Loom_Hypnos::initialize() {
    /* Set the rail pins to output mode */
    pinMode(PIN_RAIL_3V, OUTPUT);
    pinMode(PIN_RAIL_5V, OUTPUT);

    setPowerRails(railConfigAwake);
    initializeRtc();
    sdMan->initialize();
    Logger::initialize(sdMan, this);
}

void Loom_Hypnos::measure() {
    strncpy(timestamp->timeUtc, rtcExternal.now().text(), 21);
    timestamp->timeUtc[10] = 'T';
    timestamp->timeUtc[19] = 'Z';
}

void Loom_Hypnos::displayData() {
    Serial.printf(
        "Hypnos:\n"
        "    Time UTC: %s\n"
        "\n",
        timestamp->timeUtc
    );
}

/* Power Rail Control Functionality */

void Loom_Hypnos::setPowerRails(struct PowerrailConfig railConfig) {
    digitalWrite(PIN_RAIL_3V, railConfig.rail_3v);
    digitalWrite(PIN_RAIL_5V, railConfig.rail_5v);
}

void Loom_Hypnos::powerDown() {
    setPowerRails(railConfigAsleep);
}

void Loom_Hypnos::powerUp() {
    setPowerRails(railConfigAwake);
}

/* RTC */

void Loom_Hypnos::initializeRtc() {
    LOG("Initializing RTC DS3231...");

    if (!rtcExternal.begin()) {
        ERROR("Couldn't start RTC!");
        return;
    }

    /* If RTC loses power, set time manually unless the unit is deployed in the
     * field with no serial interface */
    if (rtcExternal.lostPower() && Serial) {
        LOG("RTC lost power, set the time");
        setCustomTime();
    }

    LOG("DS3231 real-time clock initialized successfully!");
    LOGF("UTC time now: %s", rtcExternal.now().text());
}

DateTime Loom_Hypnos::getCurrentTimeUtc() {
    return rtcExternal.now();
}

int16_t Loom_Hypnos::serialReadInt(const char *prompt, int16_t min, int16_t max) {
    while (true) {
        Serial.printf("%s (%d-%d)\n", prompt, min, max);

        /* Block until at least one byte arrives in the Serial buffer */
        while (Serial.available() == 0) {
            delay(10);
        }

        long valueLong = Serial.parseInt();

        /* Clear any leftover newline or carriage return characters from the
         * buffer */
        while (Serial.available()) {
            char next = Serial.peek();
            if (next != '\r' && next != '\n') {
                break;
            }
            Serial.read();
        }

        int16_t value16 = (int16_t) valueLong;
        if (valueLong < INT16_MIN
            || valueLong > INT16_MAX
            || value16 < min
            || value16 > max
        ) {
            Serial.printf("[WARNING] value %ld out of range.\n", valueLong);
            continue;
        }

        Serial.printf("Value entered: %d\n", value16);
        return value16;
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

    int16_t computerYear = serialReadInt("Enter the year", 2000, 9999);
    int16_t computerMonth = serialReadInt("Enter the month", 1, 12);
    int16_t computerDay = serialReadInt("Enter the day", 1, 31);
    int16_t computerHour = serialReadInt("Enter the hour", 0, 23);
    int16_t computerMinute = serialReadInt("Enter the minute", 0, 59);
    int16_t computerSecond = serialReadInt("Enter the second", 0, 59);

    DateTime newTimeUtc = DateTime(
        computerYear, computerMonth, computerDay,
        computerHour, computerMinute, computerSecond
    );
    rtcExternal.adjust(newTimeUtc);

    LOGF("Custom time successfully set to %s", rtcExternal.now().text());
}
