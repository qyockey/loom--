#pragma once

#include "Modules/Module.h"

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
    void writeCsvHeader1(File *csv) override;
    void writeCsvHeader2(File *csv) override;
    void writeCsvBody(File *csv) override;

  public:
    /**
     * Constructs a new Random sensor
     * @param Pointer to measured random data structure
     */
    Loom_Random(struct RandomData *random);

  private:
    struct RandomData *random;
};
