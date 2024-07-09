#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <fmt/core.h>
#include <string.h>

#include "cpu.hpp"

uint16_t parseCpuCount(const char* str) {
  // Source: https://android.googlesource.com/platform/bionic/+/refs/heads/main/libc/private/get_cpu_count_from_string.h
  int cpu_count = 0;
  int last_cpu = -1;
  while (*str != '\0') {
    if (isdigit(*str)) {
      int cpu = static_cast<int>(strtol(str, const_cast<char**>(&str), 10));
      if (last_cpu != -1) {
        cpu_count += cpu - last_cpu;
      } else {
        cpu_count++;
      }
      last_cpu = cpu;
    } else {
      if (*str == ',') {
        last_cpu = -1;
      }
      str++;
    }
  }
  return cpu_count;
}

unsigned int CpuManager::getCoreFreq(unsigned int index) {
    std::ifstream file(fmt::format("/sys/devices/system/cpu/cpu{}/cpufreq/scaling_cur_freq", index));
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
    std::ifstream file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors");
    if (!file.is_open()) {
        governors.push_back("N/A");
        return governors;
    }

    std::string governor;
    file >> governor;

    char **savePtr;
    char *token = strtok_r((char*)governor.c_str(), " ", savePtr);
    while(token != NULL) {
        governors.push_back(std::string(token));
        token = strtok_r(NULL, " ", savePtr);
    }

    if (governors.empty())
        governors.push_back("N/A");
    return governors;
}

int8_t parseThermalZoneNum(const std::string& path) {
    int num = -1;
    sscanf(path.c_str(), "thermal_zone%d", &num);
    return num;
}

void CpuManager::getCpuThermalZone() {
    for (const auto& dir : std::filesystem::directory_iterator("/sys/class/thermal/")) {
        if (!dir.is_directory() || dir.path().filename().string().find("thermal_zone") == -1)
            continue;

        std::ifstream file(dir.path() / "type");
        if (!file.is_open())
            continue;

        std::string type;
        file >> type;
        if (type == "mtktscpu") {
            this->cpuThermalZone = parseThermalZoneNum(dir.path().filename().string());
            break;
        }
    }
}

float CpuManager::getTemp() {
    if (this->cpuThermalZone == -1) {
        this->getCpuThermalZone();
        if (this->cpuThermalZone == -1)
            return -127;
    }

    std::ifstream file(fmt::format("/sys/class/thermal/thermal_zone{}/temp", this->cpuThermalZone));
    if (!file.is_open())
        return -127;

    int32_t temp;
    file >> temp;
    return (float)temp/1000;
}

std::string CpuManager::getReadableCoreFreq(unsigned int index) {
    unsigned int freq = this->getCoreFreq(index);
    if (freq == 0)
        return "Off";
    return fmt::format("{:.3} GHz", (float)freq/1000000);
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
