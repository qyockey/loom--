#pragma once

#include <Arduino.h>

/* Pins on the Feather used to control Hypnos features */
#define PIN_RAIL_3V 5U
#define PIN_RAIL_5V 6U

/* Set rail active states depending on transistor type uesd.
 * The 3V rail uses a P-type CMOS and is therefore is active low, whereas the
 * 5V rail uses an N-type CMOS and is active high */

enum Rail3vState {
    RAIL_3V_ON = LOW,
    RAIL_3V_OFF = !RAIL_3V_ON,
};

enum Rail5vState {
    RAIL_5V_ON = HIGH,
    RAIL_5V_OFF = !RAIL_5V_ON,
};

/**
 * Struct to represent power rail states
 */
struct PowerRailConfig {
    uint8_t rail_3v: 1;
    uint8_t rail_5v: 1;
};

namespace RailState {
    constexpr struct PowerRailConfig PR_3V_ON_5V_ON = {
        .rail_3v = RAIL_3V_ON,
        .rail_5v = RAIL_5V_ON,
    };
    constexpr struct PowerRailConfig PR_3V_ON_5V_OFF = {
        .rail_3v = RAIL_3V_ON,
        .rail_5v = RAIL_5V_OFF,
    };
    constexpr struct PowerRailConfig PR_3V_OFF_5V_ON = {
        .rail_3v = RAIL_3V_OFF,
        .rail_5v = RAIL_5V_ON,
    };
    constexpr struct PowerRailConfig PR_3V_OFF_5V_OFF = {
        .rail_3v = RAIL_3V_OFF,
        .rail_5v = RAIL_5V_OFF,
    };
};

/**
 * Controls 3.3V/5V power rails on the Hypnos board.
 */
class Loom_PowerRail {
  public:

    /* 
     * Initialize power rail control pins and set rails to awake configuration
     */
    void initialize(void);

    /**
     * Set the configuration for the power rails when waking up from sleep
     * @param config The desired configuration while the device is awake
     * See namespace RailState.
     */
    void setWakeConfiguration(PowerRailConfig config) {
        railConfigAwake = config;
    };

    /**
     * Set the configuration for the power rails when going to sleep
     * @param config The desired configuration while the device is asleep
     * See namespace RailState.
     */
    void setSleepConfiguration(PowerRailConfig config) {
        railConfigAsleep = config;
    };

    /**
     * Apply wake power rail configuration
     */
    void awakeMode(void);

    /**
     * Apply sleep power rail configuration
     */
    void asleepMode(void);

  private:

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
    void setPowerRails(PowerRailConfig railConfig);

    /* Power rail configuration for when the device is awake */
    PowerRailConfig railConfigAwake = RailState::PR_3V_ON_5V_ON;

    /* Power rail configuration for the when the device is asleep */
    PowerRailConfig railConfigAsleep = RailState::PR_3V_OFF_5V_OFF;
};
