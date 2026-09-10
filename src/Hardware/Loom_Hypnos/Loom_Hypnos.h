#pragma once

#include <Arduino.h>
#include <OPEnS_RTC.h>
#include <ArduinoLowPower.h>

#include "Module.h"
#include "SdManager.h"

/* Pins on the Feather used to control Hypnos features */
#define PIN_RAIL_3V 5U
#define PIN_RAIL_5V 6U
#define PIN_RTC_ALARM 12U

/* Set rail active states depending on transistor type uesd.
 * The 3V rail uses a P-type CMOS and is therefore is active low, whereas the
 * 5V rail uses an N-type CMOS and is active high */
#define RAIL_3V_ON LOW
#define RAIL_3V_OFF (!RAIL_3V_ON)
#define RAIL_5V_ON HIGH
#define RAIL_5V_OFF (!RAIL_5V_ON)

struct TimestampData {
    enum ModuleTag tag = MODULE_RTC;
    char timeUtc[21];
};

/**
 * Struct to represent power rail states
 */
struct PowerrailConfig {
    uint8_t rail_3v: 1;
    uint8_t rail_5v: 1;
};

namespace RailState {
    constexpr struct PowerrailConfig PR_3V_ON_5V_ON = {
        .rail_3v = RAIL_3V_ON,
        .rail_5v = RAIL_5V_ON,
    };
    constexpr struct PowerrailConfig PR_3V_ON_5V_OFF = {
        .rail_3v = RAIL_3V_ON,
        .rail_5v = RAIL_5V_OFF,
    };
    constexpr struct PowerrailConfig PR_3V_OFF_5V_ON = {
        .rail_3v = RAIL_3V_OFF,
        .rail_5v = RAIL_5V_ON,
    };
    constexpr struct PowerrailConfig PR_3V_OFF_5V_OFF = {
        .rail_3v = RAIL_3V_OFF,
        .rail_5v = RAIL_5V_OFF,
    };
};

/**
 * Tracks the hypnos version and matches the version with the correct chip
 * select pin
 */
enum HypnosVersion { V3_2 = 10U, V3_3 = 11U, ADALOGGER = 4U };

/**
 * Controls RTC, 3.3V/5V power rails, and SD card on the Hypnos board.
 *
 * The hypnos is treated as a module because the RTC time is a measured quantity
 */
class Loom_Hypnos : public Module{
  protected:

    /* These aren't used with the Hypnos.
     * power_down() and power_up() also *could* be implemented, but the logic is
     * cleaner and easier to understand when all inside sleep() */
    void measure() override {};
    void power_up() override {};
    void power_down() override {};

    /* Initialize power rails and RTC */
    void initialize() override;

    /* Display current time in UTC */
    void display_data() override;

    /**
     * Drops the Feather M0 and Hypnos board into a low power sleep waiting for
     * an interrupt to wake it up and pull it out of sleep.
     * This is much more power efficient for long delays than calling
     * manager.pause()
     * @param duration The time that will elapse before the device is woken by
     * the RTC
     * @param waitForSerial Whether to still execution until Serial monitor
     * reconnects after powering up.  This takes about 2 seconds.
     */
    void sleep(TimeSpan duration, bool waitForSerial = false);
    friend class Manager;

  public:

    /**
     * Constructs a new Hypnos instance and registers it with the manager
     * @param timestamp Pointer to mesured timestamp structure
     * @param version The version of the Hypnos in use, this changes which pin
     * is used as SD chip select
     * @param deviceName Name of device to use in log files
     */
    Loom_Hypnos(
        struct TimestampData *timestamp,
        HypnosVersion version,
        const char *deviceName
    );

    /* Power Control Functionality */

    /**
     * Set the configuration for the power rails when waking up from sleep
     *
     * NOTE: 3V rail should always be on while awake.
     * Because the 3V rail pulls up SDA and SCL, any attempted I2C transmission
     * while the rail is off will fail and cause the device to hang.
     * This should not be a problem under normal circumstances.
     *
     * @param config The desired configuration while the device is awake
     * See namespace RailState.
     */
    void setWakeConfiguration(PowerrailConfig config) {
        railConfigAwake = config;
    };

    /**
     * Set the configuration for the power rails when going to sleep
     * @param config The desired configuration while the device is asleep
     * See namespace RailState.
     */
    void setSleepConfiguration(PowerrailConfig config) {
        railConfigAsleep = config;
    };

    /* RTC Functionality */

    /**
     * Get the current UTC time from the RTC
     */
    DateTime getCurrentTimeUtc();

  private:

    struct TimestampData *timestamp;
    SdManager *sdMan = nullptr;
    uint8_t sdChipSelect;

    /* Power rails */

    /**
     * Set power rails to be either on or off
     *
     * NOTE: Do not attempt to transmit over I2C while the 3V rail is disabled.
     * See note above setWakeConfiguration() for more information.
     *
     * @param railConfig Configuration of how 3V and 5V rails should turn on
     * or off
     * See namespace RailState.
     */
    void setPowerRails(PowerrailConfig railConfig);

    /* Power rail configuration for when the device is awake */
    PowerrailConfig railConfigAwake = RailState::PR_3V_ON_5V_ON;
    /* Power rail configuration for the when the device is asleep */
    PowerrailConfig railConfigAsleep = RailState::PR_3V_OFF_5V_OFF;

    /* Real-Time Clock (RTC) */

    /* RTC instance */
    RTC_DS3231 RTC_DS;
    void initializeRtc();

    /* Prompt user for integer */
    int16_t serialReadInt(const char *prompt, int16_t min, int16_t max);

    /**
     * Set a custom time on startup for the RTC to use
     * This function is only automatically called
     */
    void setCustomTime();

    /* Sleep functionality */
    void setWakeupAlarm(DateTime timeAlarmUtc);
    static void wakeup();
    static volatile bool shouldPowerUp;
};
