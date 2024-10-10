#pragma once

#include <string>
#include <vector>

namespace CPU {
unsigned int getMaxCore();
std::string getGovernor();
std::vector<std::string> getGovernors();
void setGovernor(std::string governor);
float getTemp();
float getMaxTemp();
float getMinTemp();
std::string getFmtTemp();
std::string getFmtMaxTemp();
std::string getFmtMinTemp();
std::string getFmtCoreFreq(unsigned int index);
} // namespace CPU