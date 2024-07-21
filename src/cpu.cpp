#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <dirent.h>
#include <string.h>

#include "cpu.hpp"
#include "utils.hpp"
#include <fmt/core.h>

unsigned int CpuManager::getCoreFreq(unsigned int index) {
    std::ifstream file(fmt::format(
        "/sys/devices/system/cpu/cpu{}/cpufreq/scaling_cur_freq", index));
    if (!file.is_open())
        return 0;

    unsigned int freq;
    file >> freq;
    return freq;
}

std::string CpuManager::getGovernor() {
    std::ifstream file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
    if (!file.is_open())
        return "N/A";

    std::string governor;
    file >> governor;
    return governor;
}

std::vector<std::string> CpuManager::getGovernors() {
    std::vector<std::string> governors;
    std::ifstream file(
        "/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors");
    if (!file.is_open()) {
        governors.push_back("N/A");
        return governors;
    }

    std::stringstream governor;
    std::string buffer;
    governor << file.rdbuf();

    while (getline(governor, buffer, ' '))
        if (buffer.length() > 1)
            governors.push_back(buffer);

    if (governors.empty())
        governors.push_back("N/A");
    return governors;
}

void CpuManager::getCpuThermalZone() {
    DIR *dir = opendir("/sys/class/thermal/");
    if (dir == NULL)
        return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if ((entry->d_type != DT_DIR && entry->d_type != DT_LNK) ||
            strstr(entry->d_name, "thermal_zone") == NULL)
            continue;

        std::ifstream file(
            fmt::format("/sys/class/thermal/{}/type", entry->d_name));
        if (!file.is_open())
            continue;

        std::string type;
        file >> type;
        if (type == "mtktscpu") {
            this->cpuThermalZone = parseThermalZoneNum(entry->d_name);
            break;
        }
    }
    closedir(dir);
}

float CpuManager::getTemp() {
    if (this->cpuThermalZone == -1) {
        this->getCpuThermalZone();
        if (this->cpuThermalZone == -1)
            return -127;
    }

    std::ifstream file(fmt::format("/sys/class/thermal/thermal_zone{}/temp",
                                   this->cpuThermalZone));
    if (!file.is_open())
        return -127;

    int32_t temp;
    file >> temp;
    return (float)temp / 1000;
}

std::string CpuManager::getReadableCoreFreq(unsigned int index) {
    unsigned int freq = this->getCoreFreq(index);
    if (freq == 0)
        return "Off";
    return fmt::format("{:.3} GHz", (float)freq / 1000000);
}

std::string CpuManager::getReadableTemp() {
    return fmt::format("{:.3} °C", this->getTemp());
}

uint16_t CpuManager::maximumCore() {
    if (this->maxCore != 0)
        return this->maxCore;

    std::ifstream file("/sys/devices/system/cpu/present");
    if (!file.is_open())
        return 0;

    std::string str;
    file >> str;
    file.close();
    this->maxCore = parseCpuCount(str.c_str());
    return this->maxCore;
}
