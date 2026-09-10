#include "Loom_STEMMA.h"
#include "Logger.h"

Loom_STEMMA::Loom_STEMMA(struct StemmaData *data, uint8_t addr) : Module() {
    this->data = data;
    this->addr = addr;
}

void Loom_STEMMA::initialize() {
    initialized = stemma.begin(addr);
    if (!initialized) {
        ERRORF("Module STEMMA not detected at I2C address 0x%02X", addr);
    }
}

void Loom_STEMMA::measure() {
    if (!initialized) {
        return;
    }

    data->temperatureC = stemma.getTemp();
    data->capacitive = stemma.touchRead(0);
}

void Loom_STEMMA::display_data() {
    if (!initialized) {
        return;
    }

    Serial.printf("STEMMA:\n");
    Serial.printf("    temperature_C: %u\n", data->temperatureC);
    Serial.printf("    capacitive_counts: %u\n", data->capacitive);
    Serial.printf("\n");
}
