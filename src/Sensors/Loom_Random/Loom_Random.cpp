#include <Arduino.h>
#include <cstdlib>
#include <ctime>

#include "Loom_Random.h"

Loom_Random::Loom_Random(struct RandomData *random) : Module() {
    this->random = random;
}

void Loom_Random::initialize() {
    srand(time(NULL));
}

void Loom_Random::measure() {
    random->value = rand() % 100;
}

void Loom_Random::displayData() {
    Serial.printf("Random:\n");
    Serial.printf("    value: %u\n", random->value);
    Serial.printf("\n");
}
