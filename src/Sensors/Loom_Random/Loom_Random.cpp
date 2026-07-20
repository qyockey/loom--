#include <Arduino.h>
#include <cstdlib>
#include <ctime>

#include "Loom_Random.h"

Loom_Random::Loom_Random(Manager &man) : Module() {
    manInst = &man;
    manInst->registerModule(this);
}

void Loom_Random::initialize() {
    srand(time(NULL));
}

void Loom_Random::measure() {
    value = rand() % 100;
}

void Loom_Random::display_data() {
    Serial.printf("Random:\n");
    Serial.printf("    value: %u\n", value);
    Serial.printf("\n");
}
