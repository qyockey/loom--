#include <cstdint>

#include "Loom_Interrupt.h"

/* This must be volatile because it will be changed within interrupt service
 * routines. */
volatile uint8_t Loom_Interrupt::pending;

uint8_t Loom_Interrupt::enabled;

void Loom_Interrupt::initialize() {
    pending = 0;
    enabled = 0;
}

bool Loom_Interrupt::isPending(uint8_t source) {
    return (enabled & pending & source) != 0;
}

void Loom_Interrupt::setPending(uint8_t source) {
    pending |= (enabled & source);
}

void Loom_Interrupt::clearPending(uint8_t source) {
    pending &= ~source;
}

bool Loom_Interrupt::isEnabled(uint8_t source) {
    return (enabled & source) != 0;
}

void Loom_Interrupt::enable(uint8_t source) {
    enabled |= source;
}

void Loom_Interrupt::disable(uint8_t source) {
    enabled &= ~source;
}
