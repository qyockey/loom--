#include "Loom_Digital.h"

Loom_Digital::Loom_Digital(struct DigitalPinData *pin, uint8_t pinNumber)
    : Module() {
    this->pin = pin;
    pin->number = pinNumber;
};

void Loom_Digital::measure() {
    pin->state = digitalRead(pin->number);
}

void Loom_Digital::displayData() {
    Serial.printf("Digital:\n");
    Serial.printf("    D%u: %u\n", pin->number, pin->state);
    Serial.printf("\n");
}

void Loom_Digital::writeCsvHeader1(File *csv) {
    csv->printf("Digital,,");
}

void Loom_Digital::writeCsvHeader2(File *csv) {
    csv->printf("Pin,State,");
}

void Loom_Digital::writeCsvBody(File *csv) {
    csv->printf("%02u,%1u,", pin->number, pin->state);
}
