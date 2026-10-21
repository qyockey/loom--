#pragma once

#include <cstdint>

#define LOOM_MUX_ADDR_DEFAULT 0x70
#define LOOM_MUX_NUM_PORTS 8  // Number of ports on the multiplexer

/**
 * TCA9548A Low-Voltage 8-Channel I²C Switch with Reset
 * https://www.ti.com/lit/ds/symlink/tca9548a.pdf
 *
 * @author Will Richards
 */
class Loom_Multiplexer {
  public:
    /**
     * Construct a new Multiplexer
     * @param addr Multiplexer I²C address
     */
    Loom_Multiplexer(uint8_t addr = LOOM_MUX_ADDR_DEFAULT);

    /**
     * Enable a specific multiplexer port for communication
     * @param port Multiplexer port number
     * @return Status: true on success, false on failure
     */
    bool enablePort(uint8_t port);

    /**
     * Disable all multiplexer ports for communication
     * @return Status: true on success, false on failure
     */
    bool disable(void);

    /**
     * Enable any combination of multiplexer ports 0-7 for communication
     * @param port Bitmask of ports to enable
     * @return Status: true on success, false on failure
     */
    bool setBitmask(uint8_t mask);

  private:
    uint8_t addr;
};
