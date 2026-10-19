#include <Arduino.h>

#include "Loom_DS3231.h"
#include "Logger/Logger.h"
#include "Hardware/Loom_Interrupt/Loom_Interrupt.h"

#define PIN_DS3231_ALARM 12

void Loom_DS3231::detectAlarm(void) {
    Loom_Interrupt::setPending(INTERRUPT_RTC_EXTERNAL);
}

Loom_DS3231::Loom_DS3231(struct DS3231Data *timestamp) : Module() {
    this->timestamp = timestamp;
}

void Loom_DS3231::initialize() {
    LOG("Initializing RTC DS3231...");

    if (!rtc.begin()) {
        ERROR("Couldn't start RTC!");
        return;
    }

    /* If RTC loses power, set time manually unless the unit is deployed in the
     * field with no serial interface */
    if (rtc.lostPower() && Serial) {
        LOG("RTC lost power, set the time");
        setCustomTime();
    }

    pinMode(PIN_DS3231_ALARM, INPUT_PULLUP);
    attachInterrupt(PIN_DS3231_ALARM, detectAlarm, FALLING);

    LOG("DS3231 real-time clock initialized successfully!");
    LOGF("UTC time now: %s", rtc.now().text());

    initialized = true;
}

void Loom_DS3231::measure() {
    getCurrentTimeUtc(&timestamp->timeUtc);
}

char *Loom_DS3231::isoFormat(struct tm *tm) {
    static char timeBuf[TIME_SIZE];
    strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%dT%H:%M:%SZ", tm);
    return timeBuf;
}

void Loom_DS3231::displayData() {
    Serial.printf(
        "DS3231:\n"
        "    Time UTC: %s\n"
        "\n",
        isoFormat(&timestamp->timeUtc)
    );
}

void Loom_DS3231::writeCsvHeader1(File *csv) {
    csv->printf("DS3231,");
}

void Loom_DS3231::writeCsvHeader2(File *csv) {
    csv->printf("Time UTC,");
}

void Loom_DS3231::writeCsvBody(File *csv) {
    char *utcIsoFormat = isoFormat(&timestamp->timeUtc);

    /* Skip 'Z' character at end of string */
    char utcWithoutZorT[TIME_SIZE - 1];
    strncpy(utcWithoutZorT, utcIsoFormat, TIME_SIZE - 2);
    utcWithoutZorT[TIME_SIZE - 2] = '\0';

    /* Overwrite 'T' character since Excel doesn't like it */
    utcWithoutZorT[10] = ' ';

    csv->printf("%s,", utcWithoutZorT);
}

void Loom_DS3231::getCurrentTimeUtc(struct tm *timeUtc) {
    if (!initialized) {
        return;
    }

    DateTime timeNowUtcDt = rtc.now();
    timeUtc->tm_sec = timeNowUtcDt.second();
    timeUtc->tm_min = timeNowUtcDt.minute();
    timeUtc->tm_hour = timeNowUtcDt.hour();
    timeUtc->tm_mday = timeNowUtcDt.day();
    timeUtc->tm_mon = timeNowUtcDt.month() - 1;
    timeUtc->tm_year = timeNowUtcDt.year() - 1900;
}

char *Loom_DS3231::getCurrentTimeUtcIsoFormat(void) {
    struct tm timeNowUtc;
    getCurrentTimeUtc(&timeNowUtc);
    return isoFormat(&timeNowUtc);
}

void Loom_DS3231::setAlarmAtUtc(struct tm *timeAlarm) {
    rtc.setAlarm(
        ALM1_MATCH_DATE,
        timeAlarm->tm_sec,
        timeAlarm->tm_min,
        timeAlarm->tm_hour,
        timeAlarm->tm_mday
    );

    LOGF("Alarm set for %s", isoFormat(timeAlarm));
}

void Loom_DS3231::setAlarmIn(uint32_t seconds) {
    struct tm timeNowUtc;
    getCurrentTimeUtc(&timeNowUtc);

    time_t timeNowUnix = mktime(&timeNowUtc);
    time_t timeAlarmUnix = timeNowUnix + (time_t) seconds;

    struct tm *timeAlarmUtc = gmtime(&timeAlarmUnix);
    setAlarmAtUtc(timeAlarmUtc);
}

int16_t Loom_DS3231::serialReadInt(const char *prompt, int16_t min, int16_t max) {
    while (true) {
        Serial.printf("%s (%d-%d)\n", prompt, min, max);

        /* Block until at least one byte arrives in the Serial buffer */
        while (Serial.available() == false) {
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

void Loom_DS3231::setCustomTime() {
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
    rtc.adjust(newTimeUtc);

    LOGF("Custom time successfully set to %s", rtc.now().text());
}
