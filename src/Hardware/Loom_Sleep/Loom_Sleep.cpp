#include <Arduino.h>

#include "Loom_Sleep.h"

void Loom_Sleep::sleep(void) {
    /* For more information on SysTick and SCB, see
     * packages/arduino/CMSIS/4.5.0/CMSIS/Include/core_cm0plus.h */

    /* Detach USB to save power */
    USBDevice.detach();

    /* Disable SYSTICK interrupt to prevent 1024 Hz wakeups during sleep */
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;	

    /* Enter deep sleep mode */
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    /* Wait for interrupt */
    __DSB();
    __WFI();

    /* Re-enable SYSTICK interrupt to restore delay() functionality */
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;	

    /* Reattach USB to enable Serial interface */
    USBDevice.attach();
}
