#include "Loom_STEMMA.h"

Loom_STEMMA::Loom_STEMMA(struct StemmaData *data, uint8_t addr) : Module() {
    this->data = data;
    this->addr = addr;
}

void Loom_STEMMA::initialize() {
    stemma.begin(addr);
}

void Loom_STEMMA::measure() {
    data->temperatureC = stemma.getTemp();
    data->capacitive = stemma.touchRead(0);
}

void Loom_STEMMA::display_data() {
    Serial.printf("STEMMA:\n");
    Serial.printf("    temperature_C: %u\n", data->temperatureC);
    Serial.printf("    capacitive_counts: %u\n", data->capacitive);
    Serial.printf("\n");
}
