#include <Arduino.h>

#include "Loom_TippingBucket.h"
#include "Hardware/Loom_Interrupt/Loom_Interrupt.h"
#include "Hardware/Loom_RTC/Loom_RTC.h"

/* Number of milliseconds per hour */
#define LOOM_MILLIS_ONE_HOUR (60 * 60 * 1000)

volatile uint32_t Loom_TippingBucket::timestamps[LOOM_MAX_HOURLY_TIPS];
volatile uint8_t Loom_TippingBucket::indexLatest;
volatile uint8_t Loom_TippingBucket::indexEarliest;
volatile bool Loom_TippingBucket::overflow;
volatile struct TippingBucketData *Loom_TippingBucket::data;
uint32_t Loom_TippingBucket::debounceMillis;

/* FIXME: A ghost tip is counted in the next sleep cycle if two tips are counted
 * in the current cycle...? */
void Loom_TippingBucket::tipDetect(void) {
    Loom_Interrupt::setPending(INTERRUPT_TIPPING_BUCKET);
}

void Loom_TippingBucket::tipHandle(void) {
    uint32_t now = Loom_RTC::getTimestampMillis();
    uint32_t latest = timestamps[indexLatest];

    /* Software debounce: ensure at least debounceMillis have elapsed since last
     * tip */
    if (now - latest < debounceMillis) {
        return;
    }

    /* Advance index in timestamp buffer */
    indexLatest = (indexLatest + 1) % LOOM_MAX_HOURLY_TIPS;
    timestamps[indexLatest] = now;

    /* Check if buffer overflow occurred */
    if (indexLatest == indexEarliest) {
        overflow = true;
        ++indexEarliest;
    }

    ++data->totalTips;
}

Loom_TippingBucket::Loom_TippingBucket(
    struct TippingBucketData *data,
    uint32_t debounceMillis,
    uint8_t pin,
    uint32_t detectionMode,
    uint32_t triggerMode
) : Module() {
    this->data = data;
    this->debounceMillis = debounceMillis;
    this->pin = pin;
    this->detectionMode = detectionMode;
    this->triggerMode = triggerMode;
}

void Loom_TippingBucket::initialize() {
    for (uint8_t i = 0; i < LOOM_MAX_HOURLY_TIPS; i++) {
        timestamps[i] = 0;
    }
    indexLatest = 0;
    indexEarliest = 0;

    overflow = false;

    pinMode(pin, detectionMode);

    /* Detect bucket tip as interrupt */
    attachInterrupt(pin, tipDetect, triggerMode);
}

/* Get number of circular buffer entries with timestamp in the past hour */
uint32_t Loom_TippingBucket::getHourlyTips() {
    /* No tips within one hour */
    if (indexEarliest == indexLatest) {
        return 0;
    }

    uint32_t timeNow = Loom_RTC::getTimestampMillis();

    /* Search from earliest for first tip within one hour */
    for (
        uint8_t i = indexEarliest;
        i != indexLatest;
        i = (i + 1) % LOOM_MAX_HOURLY_TIPS
    ) {
        uint32_t sampleTime = timestamps[i];

        if (timeNow - sampleTime < LOOM_MILLIS_ONE_HOUR && sampleTime != 0) {
            /* Found first tip within one hour */
            indexEarliest = i;
            return (indexLatest - indexEarliest) % LOOM_MAX_HOURLY_TIPS + 1;
        }
    }

    /* No tips found within one hour */
    indexEarliest = indexLatest;
    return 0;
}

void Loom_TippingBucket::measure() {
    data->hourlyTips = getHourlyTips();
}

void Loom_TippingBucket::displayData() {
    Serial.print("TippingBucket:\n");
    Serial.printf("    Total Tips: %u\n", data->totalTips);
    Serial.printf("    Hourly Tips: %u\n", data->hourlyTips);
    Serial.print("\n");
}

void Loom_TippingBucket::writeCsvHeader1(File *csv) {
    csv->print("TippingBucket,,");
}

void Loom_TippingBucket::writeCsvHeader2(File *csv) {
    csv->print("Total Tips,Hourly Tips,");
}

void Loom_TippingBucket::writeCsvBody(File *csv) {
    csv->printf("%05u,", data->totalTips);
    csv->printf("%03u,", data->hourlyTips);
}
