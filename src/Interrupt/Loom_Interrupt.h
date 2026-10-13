#pragma once

/* All potential interrupt sources within Loom.  If another interrupt becomes
 * possible in the future, simply add another entry to the enum and the rest
 * will be taken care of. */
#define INTERRUPT_RTC_INTERNAL    0x01 /* Real-time counter on SAMD21 */
#define INTERRUPT_RTC_EXTERNAL    0x02 /* DS3231 real-time clock */
#define INTERRUPT_TIPPING_BUCKET  0x04 /* Rainfall collection tip */
#define INTERRUPT_ANEMOMETER      0x08 /* Wind speed revolution */

/**
 * Detect and monitor pending interrupts.
 * 
 * This allows interrupt service routines to be as short as possible, just a
 * single call to `setPending`.  The main loop of the application can then check
 * each interrupt independently with `isPending`, handle it as needed, then
 * clear with `clearPending`.
 *
 * @author Quinn Yockey
 */
class Loom_Interrupt {
  public:
    static void initialize(void);

    /* Check if interrupt is marked as pending */
    static bool isPending(uint8_t source);

    /* Mark interrupt as pending */
    static void setPending(uint8_t source);

    /* Mark interrupt as not pending */
    static void clearPending(uint8_t source);

    /* Check if interrupt is enabled */
    static bool isEnabled(uint8_t source);

    /* Enable pending interrupts from source */
    static void enable(uint8_t source);

    /* Disable pending interrupts from source */
    static void disable(uint8_t source);

  private:
    static volatile uint8_t pending;
    static uint8_t enabled;
};
