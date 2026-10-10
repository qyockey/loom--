#pragma once

#include "Loom_Manager.h"
#include "Modules/Module.h"

#include "Wire.h"

#define LOOM_MUX_ADDR_DEFAULT 0x70
#define LOOM_MUX_NUM_PORTS 8;  // Number of ports on the multiplexer

/**
 * TCA9548A Low-Voltage 8-Channel I2C Switch with Reset
 * https://www.ti.com/lit/ds/symlink/tca9548a.pdf
 *
 * @author Will Richards
 */
class Loom_Multiplexer : public Module {
  public:
    /* Loomified generalized calls*/
    void initialize() override;
    void measure() override;
    void package() override;
    void power_down() override;
    void power_up() override;

    /**
     * Construct a new Multiplexer
     * @param addr Multiplexer I2C address
     */
    Loom_Multiplexer(uint8_t addr = LOOM_MUX_ADDR_DEFAULT);

    /**
     * Select a multiplexer port to communicate on
     * @param port Multiplexer port number
     * @return Status: true on success, false on failure
     */
    bool selectPort(uint8_t port);

  private:
    uint8_t addr;
};
