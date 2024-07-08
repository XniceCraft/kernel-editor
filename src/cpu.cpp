#include <fstream>
#include <string>
#include <sys/sysinfo.h>
#include <fmt/core.h>
#include "cpu.hpp"

unsigned int CpuManager::getCoreFreq(unsigned int index) {
    std::ifstream file(fmt::format("/sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq", index));
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
    return fmt::format("%f GHz", freq/1000000);
}

int CpuManager::maximumCore() {
    return get_nprocs_conf();
}
