#pragma once

#include <cstdint>

/**
 */
class Loom_RTC {
  public:

    /* Initialize counter in 32-bit operation mode incrementing at roughly 1ms
     * (exact: 1024 Hz or 976 μs) */
    static void initialize(void);

    /* Get current counter value */
    static uint32_t getTicks(void);

    /* Get current timestamp in milliseconds */
    static uint32_t getTimestampMillis(void);

    /* Set alarm to go off in given number of milliseconds */
    static void setAlarm(uint32_t ms);

    /* Disable any pending alarm */
    static void unsetAlarm(void);

    /* Register a callback function to call once alarm triggers */
    static void registerAlarmCallback(void (*callback)(void));

    static void (*callback)(void);

  private:
    static void configureRtcClockSource(void);
    static uint32_t tick2ms(uint32_t tick);
    static uint32_t ms2tick(uint32_t ms);

    static bool initialized;
};
