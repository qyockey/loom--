#include "Loom_Digital.h"

Loom_Digital::Loom_Digital(struct DigitalPinData *pin, uint8_t pinNumber)
    : Module() {
    this->pin = pin;
    pin->number = pinNumber;
};

void Loom_Digital::measure() {
    pin->state = digitalRead(pin->number);
}

void Loom_Digital::display_data() {
    Serial.printf("Digital:\n");
    Serial.printf("    D%u: %u\n", pin->number, pin->state);
    Serial.printf("\n");
}
