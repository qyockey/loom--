#include <Arduino.h>

#include "Loom_PowerRail.h"

void Loom_PowerRail::initialize() {
    /* Set the rail pins to output mode */
    pinMode(PIN_RAIL_3V, OUTPUT);
    pinMode(PIN_RAIL_5V, OUTPUT);

    awakeMode();
}

void Loom_PowerRail::setPowerRails(struct PowerRailConfig railConfig) {
    digitalWrite(PIN_RAIL_3V, railConfig.rail_3v);
    digitalWrite(PIN_RAIL_5V, railConfig.rail_5v);
}

void Loom_PowerRail::awakeMode() {
    setPowerRails(railConfigAwake);
}

void Loom_PowerRail::asleepMode() {
    setPowerRails(railConfigAsleep);
}
