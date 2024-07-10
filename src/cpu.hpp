#pragma once

#include <string>
#include <vector>

class CpuManager {
  public:
    unsigned int getCoreFreq(unsigned int index);
    std::string getGovernor();
    std::vector<std::string> getGovernors();
    std::string getReadableCoreFreq(unsigned int index);
    std::string getReadableTemp();
    float getTemp();
    uint16_t maximumCore();

  protected:
    void getCpuThermalZone();
    uint16_t maxCore = 0;
    int8_t cpuThermalZone = -1;
};
