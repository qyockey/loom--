#pragma once

#include "Module.h"

struct PacketNumberData {
    uint32_t number;
};

/**
 * Records current packet count
 */
class Loom_PacketNumber : public Module {
  protected:
    void initialize() override;
    void measure() override;
    void displayData() override;
    void powerDown() override {};
    void powerUp() override {};
    void writeCsvHeader1(File *csv) override;
    void writeCsvHeader2(File *csv) override;
    void writeCsvBody(File *csv) override;

  public:
    /**
     * Records current packet count
     */
    Loom_PacketNumber(struct PacketNumberData *packet);

  private:
    struct PacketNumberData *packet;
};
