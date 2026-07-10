#include "Loom_AS7265X.h"
#include "Logger.h"

Loom_AS7265X::Loom_AS7265X(Manager &man, int addr) : Module() {
    man->registerModule(this);
}

void Loom_AS7265X::initialize() {
    asInst.begin();
    this.power_up();
}

void Loom_AS7265X::measure() {
    asInst.takeMeasurements();

    // UV
    uv[0] = asInst.getCalibratedA();
    uv[1] = asInst.getCalibratedB();
    uv[2] = asInst.getCalibratedC();
    uv[3] = asInst.getCalibratedD();
    uv[4] = asInst.getCalibratedE();
    uv[5] = asInst.getCalibratedF();

    // Color
    color[0] = asInst.getCalibratedG();
    color[1] = asInst.getCalibratedH();
    color[2] = asInst.getCalibratedI();
    color[3] = asInst.getCalibratedJ();
    color[4] = asInst.getCalibratedK();
    color[5] = asInst.getCalibratedL();

    // NIR
    nir[0] = asInst.getCalibratedR();
    nir[1] = asInst.getCalibratedS();
    nir[2] = asInst.getCalibratedT();
    nir[3] = asInst.getCalibratedU();
    nir[4] = asInst.getCalibratedV();
    nir[5] = asInst.getCalibratedW();
}

void Loom_AS7265X::display_data() {
    Serial.printf("AS7265X:\n");
    Serial.printf("    UV_410nm: %u\n", uv[0]);
    Serial.printf("    UV_435nm: %u\n", uv[1]);
    Serial.printf("    UV_460nm: %u\n", uv[2]);
    Serial.printf("    UV_485nm: %u\n", uv[3]);
    Serial.printf("    UV_510nm: %u\n", uv[4]);
    Serial.printf("    UV_535nm: %u\n", uv[5]);
    Serial.printf("    Color_560nm: %u\n", color[0]);
    Serial.printf("    Color_585nm: %u\n", color[1]);
    Serial.printf("    Color_645nm: %u\n", color[2]);
    Serial.printf("    Color_705nm: %u\n", color[3]);
    Serial.printf("    Color_900nm: %u\n", color[4]);
    Serial.printf("    Color_940nm: %u\n", color[5]);
    Serial.printf("    NIR_610nm: %u\n", nir[0]);
    Serial.printf("    NIR_680nm: %u\n", nir[1]);
    Serial.printf("    NIR_730nm: %u\n", nir[2]);
    Serial.printf("    NIR_760nm: %u\n", nir[3]);
    Serial.printf("    NIR_810nm: %u\n", nir[4]);
    Serial.printf("    NIR_860nm: %u\n", nir[5]);
    Serial.printf("\n");
}

void Loom_AS7265X::power_up() {
    asInst.setGain(64);
    asInst.setMeasurementMode(AS7265X_MEASUREMENT_MODE_6CHAN_ONE_SHOT);
    asInst.setIntegrationCycles(50);
}
