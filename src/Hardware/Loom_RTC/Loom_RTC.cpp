#include <Arduino.h>

#include "Loom_RTC.h"

bool Loom_RTC::initialized = false;
void (*Loom_RTC::callback)(void) = nullptr;

void Loom_RTC::initialize(void) {
    /* Set OSCULP32K ultra low power 32.768 kHz oscillator to clock the RTC.
     * Configure the RTC in 32-bit counter mode with each tick representing
     * 30.52 μs.
     * Enable the RTC to generate interrupts. */

    /* Configure OSCULP32K to control GCLK6 */
    GCLK->GENCTRL.reg = (
          GCLK_GENCTRL_ID(6)
        | GCLK_GENCTRL_GENEN
        | GCLK_GENCTRL_SRC_OSCULP32K
    );
    while (GCLK->STATUS.bit.SYNCBUSY == 1);

    /* Configure GCLK6 to control GCLK_RTC */
    GCLK->CLKCTRL.reg = (
          GCLK_CLKCTRL_GEN_GCLK6
        | GCLK_CLKCTRL_ID_RTC
        | GCLK_CLKCTRL_CLKEN
    );
    while (GCLK->STATUS.bit.SYNCBUSY == 1);

    /* Disable RTC during configuration */
    RTC->MODE0.CTRL.bit.ENABLE = 0;
    while (RTC->MODE0.STATUS.bit.SYNCBUSY == 1);

    /* Configure 32-bit counter (MODE0) with no clock division (prescalar 1) */
    RTC->MODE0.CTRL.reg = (
          RTC_MODE0_CTRL_MODE_COUNT32
        | RTC_MODE0_CTRL_PRESCALER_DIV1
    );

    /* Enable RTC */
    RTC->MODE0.CTRL.bit.ENABLE = 1;
    while (RTC->MODE0.STATUS.bit.SYNCBUSY == 1);

    /* Enable RTC interrupt in NVIC table */
    NVIC_EnableIRQ(RTC_IRQn);
}

uint32_t Loom_RTC::ms2tick(uint32_t ms) {
    /* 1 ms = 32.768 ticks @ 32.768 kHz */
    return (uint32_t) (
        ((uint64_t)(ms) * (uint64_t)(32768))
        / (uint64_t)(1000)
    );
}

uint32_t Loom_RTC::tick2ms(uint32_t tick) {
    /* 1 ms = 32.768 ticks @ 32.768 kHz */
    return (uint32_t) (
        ((uint64_t)(tick) * (uint64_t)(1000))
        / (uint64_t)(32768)
    );
}

uint32_t Loom_RTC::getTicks(void) {
    return RTC->MODE0.COUNT.reg;
}

uint32_t Loom_RTC::getTimestampMillis(void) {
    return Loom_RTC::tick2ms(RTC->MODE0.COUNT.bit.COUNT);
}

void Loom_RTC::setAlarm(uint32_t ms) {
    /* Set tick compare value */
    RTC->MODE0.COMP[0].reg = RTC->MODE0.COUNT.bit.COUNT + Loom_RTC::ms2tick(ms);

    /* Enable interrupt on compare match */
    RTC->MODE0.INTENSET.bit.CMP0 = 1;
}

void Loom_RTC::unsetAlarm(void) {
    /* Disable compare match interrupt */
    RTC->MODE0.INTENCLR.bit.CMP0 = 1;
}

void Loom_RTC::registerAlarmCallback(void (*callback)(void)) {
    Loom_RTC::callback = callback;
}

void RTC_Handler(void) {
    if (Loom_RTC::callback != nullptr) {
        Loom_RTC::callback();
    }

    /* Clear interrupt pending flag */
    RTC->MODE0.INTFLAG.bit.CMP0 = 1;
}
