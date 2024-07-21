#pragma once

#include <string>

class BatteryManager {
  public:
    std::string getFmtCurrent();
    std::string getFmtTemp();
    std::string getFmtVoltage();
    std::string getHealth();
    int16_t getCurrent();
    float getTemp();
    float getVoltage();
};
