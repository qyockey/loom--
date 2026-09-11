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

void Loom_Analog::displayData() {
    Serial.printf(
        "Analog:\n"
        "    Pin: A%u%s\n"
        "    mV: %u\n"
        "    ADC Code: %u\n"
        "\n",
        pin->number, (pin->number == PIN_VBAT) ? " (VBAT)" : "",
        pin->mv, pin->analogCode
    );
}

/* Convert ADC code to voltage in millivolts */
uint16_t Loom_Analog::analogToMV(uint16_t analogCode) {
    float vrefFraction = analogCode / (float)ADC_MAX_CODE;
    float pinVoltage = vrefFraction * ADC_VREF;

    /* Convert from volts to millivolts */
    return (uint16_t) (pinVoltage * 1000.0F);
}

void Loom_Analog::writeCsvHeader1(File *csv) {
    csv->printf("Analog,,,");
}

void Loom_Analog::writeCsvHeader2(File *csv) {
    csv->printf("Pin,ADC Code,Millivolts,");
}

void Loom_Analog::writeCsvBody(File *csv) {
    csv->printf(
        "%02u,%04u,%04u,",
        pin->number, pin->analogCode, pin->mv
    );
}
