#include "Loom_AS7265X.h"
#include "Logger.h"

Loom_AS7265X::Loom_AS7265X(struct AS7265XData *data, uint8_t addr)
    : Module() {
    this->data = data;
    this->addr = addr;
}

void Loom_AS7265X::initialize() {
    initialized = asInst.begin();
    if (!initialized) {
        ERRORF("Module AS7265X not detected at I2C address 0x%02X", addr);
        return;
    }

    asInst.setGain(64);
    asInst.setMeasurementMode(AS7265X_MEASUREMENT_MODE_6CHAN_ONE_SHOT);
    asInst.setIntegrationCycles(50);
}

void Loom_AS7265X::measure() {
    if (!initialized) {
        return;
    }

    asInst.takeMeasurements();

    // UV
    data->uv[0] = asInst.getCalibratedA();
    data->uv[1] = asInst.getCalibratedB();
    data->uv[2] = asInst.getCalibratedC();
    data->uv[3] = asInst.getCalibratedD();
    data->uv[4] = asInst.getCalibratedE();
    data->uv[5] = asInst.getCalibratedF();

    // Color
    data->color[0] = asInst.getCalibratedG();
    data->color[1] = asInst.getCalibratedH();
    data->color[2] = asInst.getCalibratedI();
    data->color[3] = asInst.getCalibratedJ();
    data->color[4] = asInst.getCalibratedK();
    data->color[5] = asInst.getCalibratedL();

    // NIR
    data->nir[0] = asInst.getCalibratedR();
    data->nir[1] = asInst.getCalibratedS();
    data->nir[2] = asInst.getCalibratedT();
    data->nir[3] = asInst.getCalibratedU();
    data->nir[4] = asInst.getCalibratedV();
    data->nir[5] = asInst.getCalibratedW();
}

void Loom_AS7265X::displayData() {
    if (!initialized) {
        return;
    }

    Serial.printf("AS7265X:\n");
    Serial.printf("    UV_410nm: %u\n", data->uv[0]);
    Serial.printf("    UV_435nm: %u\n", data->uv[1]);
    Serial.printf("    UV_460nm: %u\n", data->uv[2]);
    Serial.printf("    UV_485nm: %u\n", data->uv[3]);
    Serial.printf("    UV_510nm: %u\n", data->uv[4]);
    Serial.printf("    UV_535nm: %u\n", data->uv[5]);
    Serial.printf("    Color_560nm: %u\n", data->color[0]);
    Serial.printf("    Color_585nm: %u\n", data->color[1]);
    Serial.printf("    Color_645nm: %u\n", data->color[2]);
    Serial.printf("    Color_705nm: %u\n", data->color[3]);
    Serial.printf("    Color_900nm: %u\n", data->color[4]);
    Serial.printf("    Color_940nm: %u\n", data->color[5]);
    Serial.printf("    NIR_610nm: %u\n", data->nir[0]);
    Serial.printf("    NIR_680nm: %u\n", data->nir[1]);
    Serial.printf("    NIR_730nm: %u\n", data->nir[2]);
    Serial.printf("    NIR_760nm: %u\n", data->nir[3]);
    Serial.printf("    NIR_810nm: %u\n", data->nir[4]);
    Serial.printf("    NIR_860nm: %u\n", data->nir[5]);
    Serial.printf("\n");
}
