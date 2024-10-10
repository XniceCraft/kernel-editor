#include "cpu.hpp"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fmt/core.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace {
static unsigned int maxCore = 0;
static std::vector<std::string> governors = CPU::getGovernors();
static float minTemp = 200;
static float maxTemp = -127;

unsigned int getCoreFreq(unsigned int index) {
    std::ifstream file(fmt::format(
        "/sys/devices/system/cpu/cpu{}/cpufreq/scaling_cur_freq", index));
    if (!file.is_open())
        return 0;

    unsigned int freq;
    file >> freq;
    return freq;
}

int8_t parseThermalZoneNum(const char *path) {
    int num = -1;
    sscanf(path, "thermal_zone%d", &num);
    return num;
}

int8_t getThermalZone() {
    DIR *dir = opendir("/sys/class/thermal/");
    if (dir == NULL)
        return -1;

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
        if (type == "mtktscpu" || type == "x86_pkg_temp")
            return parseThermalZoneNum(entry->d_name);
    }
    closedir(dir);
    return -1;
}

uint16_t parseCpuCount(const char *str) {
    // Source:
    // https://android.googlesource.com/platform/bionic/+/refs/heads/main/libc/private/get_cpu_count_from_string.h
    int cpu_count = 0;
    int last_cpu = -1;
    while (*str != '\0') {
        if (isdigit(*str)) {
            int cpu =
                static_cast<int>(strtol(str, const_cast<char **>(&str), 10));
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

static int8_t thermalZone = getThermalZone();
} // namespace

namespace CPU {
unsigned int getMaxCore() {
    if (maxCore != 0)
        return maxCore;

    std::ifstream file("/sys/devices/system/cpu/present");
    if (!file.is_open())
        return 0;

    std::string str;
    file >> str;
    file.close();
    maxCore = parseCpuCount(str.c_str());
    return maxCore;
}

std::string getGovernor() {
    std::ifstream file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
    if (!file.is_open())
        return "N/A";

    std::string governor;
    file >> governor;
    return governor;
}

std::vector<std::string> getGovernors() {
    if (!governors.empty())
        return governors;

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

void setGovernor(std::string governor) {
    std::ofstream file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
    if (!file.is_open() || std::find(governors.begin(), governors.end(),
                                     governor) == governors.end())
        return;

    file << governor;
    file.close();
}

float getTemp() {
    if (thermalZone == -1)
        return -127;

    std::ifstream file(
        fmt::format("/sys/class/thermal/thermal_zone{}/temp", thermalZone));
    if (!file.is_open())
        return -127;

    int32_t temp;
    file >> temp;
    float result = (float)temp / 1000;
    if (minTemp > result)
        minTemp = result;
    if (maxTemp < result)
        maxTemp = result;
    return result;
}

float getMaxTemp() { return maxTemp; }
float getMinTemp() { return minTemp; }

std::string getFmtTemp() { return fmt::format("{:.3} °C", getTemp()); }
std::string getFmtMaxTemp() { return fmt::format("{:.3} °C", maxTemp); }
std::string getFmtMinTemp() { return fmt::format("{:.3} °C", minTemp); }

std::string getFmtCoreFreq(unsigned int index) {
    unsigned int freq = getCoreFreq(index);
    if (freq == 0)
        return "Off";
    return fmt::format("{} MHz", freq / 1000);
}
} // namespace CPU