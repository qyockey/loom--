#pragma once

#include "../../Loom_Manager.h"

/**
 * Random Sensor
 */
class Loom_Random : public Module {
  protected:
    // Manager controlled functions
    void measure() override;
    void initialize() override;
    void power_up() override;
    void power_down() override;
    void display_data() override;

  public:
    /**
     * Constructs a new Random sensor
     * @param man Reference to the manager that is used to universally package all data
     */
    Loom_Random(Manager &man);

  private:
    Manager *manInst;
    bool device_on;
    uint8_t value;
};
