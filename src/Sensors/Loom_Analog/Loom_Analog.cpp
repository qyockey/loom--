#include "Loom_Analog.h"
#include "Logger.h"

Loom_Analog::Loom_Analog(Manager &man) : Module() {
    /* Register the module with the manager */
    man.registerModule(this);
};

void Loom_Analog::initialize() {
    /* Set all pins inactive */
    for (uint8_t pinNumber = 0; pinNumber < MAX_ANALOG_PINS; pinNumber++) {
        pinMappings[pinNumber].active = false;
    }

    /* Configure ADC to mesure to desired resolution */
    analogReadResolution(ADC_RESOLUTION_BITS);

    /* Add battery voltage to measurement list */
    addMeasuredPin(PIN_VBAT);
}

void Loom_Analog::addMeasuredPin(uint8_t pin) {
    if (pin >= MAX_ANALOG_PINS) {
        ERRORF(
            "Cannot measure analog pin %u, max is %u\n",
            pin, MAX_ANALOG_PINS - 1
        );
    }

    pinMappings[pin].active = true;
}

void Loom_Analog::measure() {
    /* Read the data from the given analog pin */
    for (uint8_t pinNumber = 0; pinNumber < MAX_ANALOG_PINS; pinNumber++) {
        AnalogMapping *pinMapping = &pinMappings[pinNumber];

        if (!pinMapping->active) {
            continue;
        }

        /* Read the ADC code convert to millivolts */
        uint16_t analogCode = analogRead(pinNumber);
        pinMapping->analogCode = analogCode;
        pinMapping->analogMv = analogToMV(analogCode);

        /* Battery is halved by voltage divider; restore it */
        if (pinNumber == PIN_VBAT) {
            pinMapping->analogMv *= 2;
        }
    }
}

void Loom_Analog::display_data() {
    Serial.printf("Analog:\n");
    for (uint8_t pinNumber = 0; pinNumber < MAX_ANALOG_PINS; pinNumber++) {
        const AnalogMapping *pinMapping = &pinMappings[pinNumber];
        if (!pinMapping->active) {
            continue;
        }

        Serial.printf("    A%u", pinNumber);

        if (pinNumber == PIN_VBAT) {
            Serial.printf(" (Vbat)");
        }

        Serial.printf(
            ": %u mV (code %u)\n",
            pinMapping->analogMv, pinMapping->analogCode
        );
    }
    Serial.printf("\n");
}

/* Convert ADC code to voltage in millivolts */
uint16_t Loom_Analog::analogToMV(uint16_t analogCode) {
    float vrefFraction = analogCode / (float)ADC_MAX_CODE;
    float pinVoltage = vrefFraction * ADC_VREF;

    /* Convert from volts to millivolts */
    return (uint16_t) (pinVoltage * 1000.0F);
}

uint16_t Loom_Analog::getMv(uint8_t targetPin) {
    struct AnalogMapping *pinMapping = &pinMappings[targetPin];

    if (!pinMapping->active) {
        return UINT16_MAX;
    }

    return pinMapping->analogMv;
}

uint16_t Loom_Analog::getAnalogCode(uint8_t targetPin) {
    struct AnalogMapping *pinMapping = &pinMappings[targetPin];

    if (!pinMapping->active) {
        return UINT16_MAX;
    }

    return pinMapping->analogCode;
}
