#include <iostream>
#include <fstream>
#include <string>
#include <sys/sysinfo.h>
#include <fmt/core.h>
#include "cpu.hpp"

uint16_t parseCpuCount(const char* str) {
    
}

unsigned int CpuManager::getCoreFreq(unsigned int index) {
    std::ifstream file(fmt::format("/sys/devices/system/cpu/cpu{}/cpufreq/scaling_cur_freq", index));
    if (!file.is_open())
        return 0;

    unsigned int freq;
    file >> freq;
    return freq;
}

std::string CpuManager::getReadableCoreFreq(unsigned int index) {
    unsigned int freq = this->getCoreFreq(index);
    if (freq == 0)
        return "Off";
    return fmt::format("{:.3} GHz", (float)freq/1000000);
}

uint16_t CpuManager::maximumCore() {
    if (this->maxCore != 0)
        return this->maxCore;
    FILE* 
    return this->maxCore;
}
