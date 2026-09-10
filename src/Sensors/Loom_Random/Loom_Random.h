#pragma once

#include "Module.h"

struct RandomData {
    uint8_t value;
};

/**
 * Random Sensor
 */
class Loom_Random : public Module {
  protected:
    void measure() override;
    void initialize() override;
    void displayData() override;
    void powerDown() override;
    void powerUp() override;

  public:
    /**
     * Constructs a new Random sensor
     * @param Pointer to measured random data structure
     */
    Loom_Random(struct RandomData *random);

  private:
    struct RandomData *random;
};
