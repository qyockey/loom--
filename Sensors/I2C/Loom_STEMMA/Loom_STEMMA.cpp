#include "Loom_STEMMA.h"
#include "Logger.h"


Loom_STEMMA::Loom_STEMMA(Manager &man, int addr) : Module(), address(addr) {
    manInst->registerModule(this);
}


void Loom_STEMMA::initialize() {
    stemma.begin(address);
}


void Loom_STEMMA::measure() {
    temperature = stemma.getTemp();
    cap = stemma.touchRead(0);
}


void Loom_STEMMA::display_data() {
    Serial.printf("STEMMA:\n");
    Serial.printf("    temperature_C: %f\n", temperature);
    Serial.printf("    capacitive_counts: %u\n", cap);
    Serial.printf("\n");
}
