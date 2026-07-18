#include <cstdlib>
#include <ctime>

#include "Loom_Random.h"

Loom_Random::Loom_Random(Manager &man, uint8_t maximum) : Module() {
    man->registerModule(this);
    this.maximum = maximum;
}

void Loom_Random::initialize() {
    srand(time(NULL));
    this.power_up();
}

void Loom_Random::measure() {
    if (!device_on) {
        value = 0;
    } else {
        value = rand() % maximum;
    }
}

void Loom_Random::display_data() {
    Serial.printf("Random:\n");
    Serial.printf("    value: %u", value);
    Serial.printf("\n");
}

void Loom_Random::power_up() {
    device_on = true;
}

void Loom_Random::power_down() {
    device_on = false;
}
