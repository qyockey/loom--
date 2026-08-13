#pragma once

#include <OPEnS_RTC.h>
#include <ArduinoLowPower.h>

#include "Arduino.h"
#include "Module.h"

#include "Loom_Manager.h"

#define PIN_RAIL_3V 5
#define PIN_RAIL_5V 6
#define PIN_RTC_ALARM 12

/* Set rail active states depending on transistor type uesd */
#define RAIL_3V_ON LOW
#define RAIL_3V_OFF HIGH
#define RAIL_5V_ON HIGH
#define RAIL_5V_OFF LOW

/**
 * Enum to represent all power rail configurations
 */
enum PowerrailConfig {
    PR_3V_ON_5V_ON   = (RAIL_3V_ON  << 1) | RAIL_5V_ON,
    PR_3V_ON_5V_OFF  = (RAIL_3V_ON  << 1) | RAIL_5V_OFF,
    PR_3V_OFF_5V_ON  = (RAIL_3V_OFF << 1) | RAIL_5V_ON,
    PR_3V_OFF_5V_OFF = (RAIL_3V_OFF << 1) | RAIL_5V_OFF,
};

/**
 * Controls RTC, 3.3V/5V power rails, and SD card on the Hypnos board
 */
class Loom_Hypnos : public Module{
  protected:

    /* These aren't used with the Hypnos */
    void measure() override {};
    void power_up() override {};
    void power_down() override {};

    /* Initialize RTC and power rails */
    void initialize() override;

    /* Display current time */
    void display_data() override;

  public:

    /**
     * Constructs a new Hypnos Instance using the manager to hold information about the device
     * @param man Reference to the manager
     */
    Loom_Hypnos(Manager& man); 

    /* Power Control Functionality */

    /**
     * Set power rails to be either on or off
     * Note: Do not attempt to transmit over I2C while the 3V rail is disabled.
     * This will cause the device to hang because the 3V rail pulls up SCL & SDA
     * @param railConfig Configuration of how 3V and 5V rails should turn on on off
     */
    void setPowerRails(PowerrailConfig railConfig);

    /**
     * Set the configuration for the power rails when going to sleep
     * @param config The desired configuration while the device is asleep
     * See enum PowerrailConfig.
     */
    void setSleepConfiguration(PowerrailConfig config) {
        railConfigAsleep = config;
    };

    /* Sleep Functionality */

    /**
     * Drops the Feather M0 and Hypnos board into a low power sleep waiting for
     * an interrupt to wake it up and pull it out of sleep.
     * This is much more power efficient for long delays than calling
     * manager.pause()
     * @param duration The time that will elapse before the device is woken by
     * the RTC
     */
    void sleep(TimeSpan duratiwon);

    /* RTC Functionality */

    /**
     * Get the current UTC time from the RTC
     */
    DateTime getCurrentTimeUtc();

  private:

    Manager* manInst = nullptr;

    /* Real-Time Clock Settings */

    RTC_DS3231 RTC_DS;  // Real time clock reference
    void initializeRTC();

    DateTime timeNowUtc; // Latest measured time
    DateTime timeAlarmUtc; // Time the alarm has been set for

    char timeString[21]; // Buffer to write time strings into
    void dateTime_toString(DateTime time, char *timeString);
    void dateTime_print(DateTime time);
    /**
     * Set a custom time on startup for the RTC to use
     * This function is only automatically called
     */
    void setCustomTime();

    /* Sleep functionality */
    static void wakeup();
    void setWakeupAlarm(DateTime timeAlarmUtc);
};

