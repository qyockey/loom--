#include <Arduino.h>

#include "Loom_Interrupt.h"

/* This must be volatile because it will be changed within interrupt service
 * routines. */
volatile uint8_t Loom_Interrupt::pending;

uint8_t Loom_Interrupt::enabled;

/* Clock external interrupt controller with 32kHz ultra-low power oscillator
 * during standby to enable asynchronous interrupts while asleep */
void enableSleepOcillator() {
    /* Dummy call to trick Arduino into initializing GCLK now before applying
     * our own configuration.  Otherwise the first call to attachInterrupt will
     * overwrite the configuration. */
    attachInterrupt(1, nullptr, DEFAULT);

    /* Configure OSCULP32K to control GCLK6 */
	GCLK->GENCTRL.reg = (
          GCLK_GENCTRL_ID(6)
        | GCLK_GENCTRL_GENEN
        | GCLK_GENCTRL_SRC_OSCULP32K
    );
	while (GCLK->STATUS.bit.SYNCBUSY == 1);

    /* Configure GCLK6 to control GCLK_EIC */
	GCLK->CLKCTRL.reg = (
          GCLK_CLKCTRL_GEN_GCLK6
        | GCLK_CLKCTRL_ID(GCM_EIC)
        | GCLK_CLKCTRL_CLKEN
    );
	while (GCLK->STATUS.bit.SYNCBUSY == 1);
}

void Loom_Interrupt::initialize() {
    pending = 0;
    enabled = 0;
    enableSleepOcillator();
}

bool Loom_Interrupt::anyPending() {
    return (enabled & pending) != 0;
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
