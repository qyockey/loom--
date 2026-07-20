#include "Loom_Analog.h"

void Loom_Analog::measure() {

    // Read the data from the given analog pin
    for (size_t i = 0; i < pinMappings.size(); i++) {

        /* If we are measuring the Vbat pin we want a little different behavior */
        if (pinMappings[i]->pinNumber == A7) {
            pinMappings[i]->analog = getBatteryVoltage();
            pinMappings[i]->analog_mv = getBatteryVoltage() * 1000;
        }

        /* If its a normal pin then just read the value and update the previous values */
        else {
            int analogData = analogRead(pinMappings[i]->pinNumber);
            pinMappings[i]->analog = analogData;
            pinMappings[i]->analog_mv = analogToMV(analogData);
        }
    }
}

void Loom_Analog::display_data() {
    Serial.printf("Analog:\n");
    for (size_t i = 0; i < pinMappings.size(); i++) {
        Serial.printf("%s: %f\n", pinMappings[i]->name, pinMappings[i]->analog);
        Serial.printf("%s_MV: %f\n", pinMappings[i]->name, pinMappings[i]->analog_mv);
    }
}

float Loom_Analog::getBatteryVoltage() {
    float pin_reading = analogRead(A7);
    pin_reading *= 2;
    pin_reading *= 3.3;
    pin_reading /= 4096;
    return pin_reading;
}

float Loom_Analog::analogToMV(int analog) {
    float analogRes = 4095.0;
    float voltage = (analog * 3.3) / analogRes;
    return voltage * 1000;
}

float Loom_Analog::getMV(int pin) {
    for (size_t i = 0; i < pinMappings.size(); i++) {
        if (pinMappings[i]->pinNumber == pin) {
            return pinMappings[i]->analog_mv;
        }
    }
    return NAN;
}

float Loom_Analog::getAnalog(int pin) {
    for (size_t i = 0; i < pinMappings.size(); i++) {
        if (pinMappings[i]->pinNumber == pin) {
            return pinMappings[i]->analog;
        }
    }
    return NAN;
}
