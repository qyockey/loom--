#pragma once

#include <OPEnS_RTC.h>
#include <time.h>

#include "Module.h"

#define TIME_SIZE 21U

struct DS3231Data {
    struct tm timeUtc;
};

/**
 * DS3231 Real-Time Clock
 */
class Loom_DS3231 : public Module {
  protected:
    void measure() override;
    void initialize() override;
    void displayData() override;
    void powerDown() override {};
    void powerUp() override {};
    void writeCsvHeader1(File *csv) override;
    void writeCsvHeader2(File *csv) override;
    void writeCsvBody(File *csv) override;

  public:
    /**
     * Constructs a new DS3231 sensor
     * @param data Pointer to DS3231 measured data structure
     */
    Loom_DS3231(struct DS3231Data *timestamp);

    /**
     * Populate time structure with current UTC time from the RTC
     * @param tm Time structure.  See time.h.
     */
    void getCurrentTimeUtc(struct tm *tm);

  private:
    DS3231Data *timestamp;

    RTC_DS3231 rtc;
    bool initialized;

    void initializeRtc();

    /* Prompt user for integer */
    int16_t serialReadInt(const char *prompt, int16_t min, int16_t max);

    /**
     * Set a custom time on startup for the RTC to use
     * This function is only automatically called
     */
    void setCustomTime();
};

