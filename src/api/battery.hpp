#pragma once

#include <string>

namespace Battery {
unsigned int getBatteryCount();
std::string getFmtCurrent(unsigned int index);
std::string getFmtTemp(unsigned int index);
std::string getFmtVoltage(unsigned int index);
std::string getHealth(unsigned int index);
float getTemp(unsigned int index);
}; // namespace Battery
