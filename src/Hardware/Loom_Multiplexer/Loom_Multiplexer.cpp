#include "Loom_Multiplexer.h"
#include "Logger.h"
#include "Wire.h"

Loom_Multiplexer::Loom_Multiplexer(uint8_t addr) {
    this->addr = addr;
}

bool Loom_Multiplexer::selectPort(uint8_t port) {
    if (port >= LOOM_MUX_NUM_PORTS) {
        return false;
    }

    Wire.beginTransmission(addr);
    Wire.write(1 << port);
    bool status = (Wire.endTransmission() == 0);

    delay(50);

    return status
}
