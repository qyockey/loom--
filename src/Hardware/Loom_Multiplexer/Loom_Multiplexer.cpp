#include "Loom_Multiplexer.h"
#include "Logger/Logger.h"
#include <Wire.h>

Loom_Multiplexer::Loom_Multiplexer(uint8_t addr) {
    this->addr = addr;
}

bool Loom_Multiplexer::setBitmask(uint8_t mask) {
    Wire.beginTransmission(addr);
    Wire.write(mask);
    uint8_t status = Wire.endTransmission();

    /* From Wire.h
     * 0: Success
     * 1: Data too long
     * 2: NACK on transmit of address
     * 3: NACK on transmit of data
     * 4: Other error
     */
    switch (status) {
        case 0:
        case 1:
            break;
        case 2:
            WARNINGF("NACK on transmit of multiplexer address 0x%02X", addr);
            break;
        case 3:
            WARNINGF("NACK on transmit of bitmask data 0x%02X", mask);
            break;
        default:
            WARNING("I²C error during transmission");
            break;
    }
    return (status == 0);
}

bool Loom_Multiplexer::enablePort(uint8_t port) {
    return setBitmask(1 << port);
}

bool Loom_Multiplexer::disable(void) {
    return setBitmask(0);
}
