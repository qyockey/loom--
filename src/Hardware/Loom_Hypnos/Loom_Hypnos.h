#pragma once

#include <OPEnS_RTC.h>
#include <ArduinoLowPower.h>

#include "Arduino.h"
#include "Module.h"

#include "Loom_Manager.h"

#define PIN_3V_RAIL 5
#define PIN_5V_RAIL 6
#define PIN_RTC_ALARM 12

/* Set rail active states depending on transistor type uesd */
#define 3V_RAIL_ON LOW
#define 3V_RAIL_OFF HIGH
#define 5V_RAIL_ON HIGH
#define 5V_RAIL_OFF LOW

/**
 * 
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

    /* RTC Functionality */

    /**
     * Get the current time in UTC from the RTC
     */
    DateTime getCurrentTimeUtc();

    /**
     * Set a custom time on startup for the RTC to use
    */
    void setCustomTime();

  private:

    Manager* manInst = nullptr;                                                         // Instance of the manager

    /* Real-Time Clock Settings */

    RTC_DS3231 RTC_DS;                                                                  // Real time clock reference
    void initializeRTC();                                                               // Initialize RTC

    DateTime timeNowUtc; // Latest measured time
    DateTime timeAlarmUtc; // Time the alarm has been set for

    char timeString[21]; // Buffer to write time strings into
    void dateTime_toString(DateTime time, char *timeString);
    void dateTime_print(DateTime time);
};
