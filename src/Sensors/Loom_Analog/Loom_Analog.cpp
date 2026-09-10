#include "Loom_Analog.h"

Loom_Analog::Loom_Analog(struct AnalogPinData *pin, uint8_t pinNumber)
    : Module() {
    this->pin = pin;
    pin->number = pinNumber;
};

void Loom_Analog::initialize() {
    /* Configure ADC to mesure to desired resolution */
    analogReadResolution(ADC_RESOLUTION_BITS);
}

void Loom_Analog::measure() {
    /* Read the ADC code convert to millivolts */
    pin->analogCode = analogRead(pin->number);
    pin->mv = analogToMV(pin->analogCode);

    /* Battery is halved by voltage divider; restore it */
    if (pin->number == PIN_VBAT) {
        pin->mv *= 2;
    }
}

void Loom_Analog::display_data() {
    Serial.printf("Analog:\n");
    Serial.printf("    A%u", pin->number);

    if (pin->number == PIN_VBAT) {
        Serial.printf(" (Vbat)");
    }

    Serial.printf(": %u mV (code %u)\n\n", pin->mv, pin->analogCode);
}

/* Convert ADC code to voltage in millivolts */
uint16_t Loom_Analog::analogToMV(uint16_t analogCode) {
    float vrefFraction = analogCode / (float)ADC_MAX_CODE;
    float pinVoltage = vrefFraction * ADC_VREF;

    /* Convert from volts to millivolts */
    return (uint16_t) (pinVoltage * 1000.0F);
}
