#include "Loom_PacketNumber.h"

Loom_PacketNumber::Loom_PacketNumber(struct PacketNumberData *packet) : Module() {
    this->packet = packet;
};

void Loom_PacketNumber::initialize() {
    packet->number = 0;
}

void Loom_PacketNumber::measure() {
    ++packet->number;
}

void Loom_PacketNumber::displayData() {
    Serial.printf(
        "PacketNumber:\n"
        "    Number: %u\n"
        "\n", packet->number
    );
}

void Loom_PacketNumber::writeCsvHeader1(File *csv) {
    csv->print("PacketNumber,");
}

void Loom_PacketNumber::writeCsvHeader2(File *csv) {
    csv->print("Number,");
}

void Loom_PacketNumber::writeCsvBody(File *csv) {
    csv->printf("%06u,", packet->number);
}
