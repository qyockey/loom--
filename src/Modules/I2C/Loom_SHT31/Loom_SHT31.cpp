#include "Loom_SHT31.h"
#include "Logger/Logger.h"

Loom_SHT31::Loom_SHT31(struct SHT31Data *data, uint8_t addr, int8_t muxPort)
    : Module() {
    this->data = data;
    this->addr = addr;
    this->muxPort = muxPort;
}

void Loom_SHT31::initialize() {
    initialized = sht.begin(addr);
    if (!initialized) {
        ERROR("Failed to initialize SHT31! Check connections and try again");
        return;
    }
    LOG("Successfully initialized SHT31!");
}

void Loom_SHT31::measure() {
    if (!initialized){
        return;
    }

    // Pull the data from the sensor
    sht.readBoth(&data->temperatureC, &data->humidity);

    // If both the temp and humidity values are valid send the data
    if (isnan(data->temperatureC) || isnan(data->humidity)) {
        WARNING("Collected data is invalid");
    }
}

void Loom_SHT31::displayData() {
    if (!initialized){
        return;
    }

    Serial.printf("SHT31:\n");
    Serial.printf("    Temperature °C: %0.2f\n", data->temperatureC);
    Serial.printf("    Humidity %RH: %0.2f\n", data->humidity);
    Serial.printf("\n");
}

void Loom_SHT31::writeCsvHeader1(File *csv) {
    csv->printf("SHT31,,");
}

void Loom_SHT31::writeCsvHeader2(File *csv) {
    csv->printf("Temperature °C,Humidity %RH,");
}

void Loom_SHT31::writeCsvBody(File *csv) {
    csv->printf("%0.2f,%0.2f,", data->temperatureC, data->humidity);
}
