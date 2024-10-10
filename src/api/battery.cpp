#include "battery.hpp"
#include <dirent.h>
#include <fmt/core.h>
#include <fstream>
#include <string>
#include <vector>

namespace {
std::vector<std::string> getBatteries() {
    std::vector<std::string> batt;
    DIR *dir = opendir("/sys/class/power_supply/");
    if (dir == NULL)
        return batt;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if ((entry->d_type != DT_DIR && entry->d_type != DT_LNK) ||
            (strcmp(entry->d_name, "battery") != 0 &&
             strstr(entry->d_name, "BAT") == NULL))
            continue;
        batt.push_back(std::string(entry->d_name));
    }
    closedir(dir);
    return batt;
}
static std::vector<std::string> batteries = getBatteries();
static std::string getHealthInternal(unsigned int index) {
    std::ifstream file(
        fmt::format("/sys/class/power_supply/{}/health", batteries[index]));
    if (file.is_open()) {
        std::string health;
        file >> health;
        return health;
    }

    int max_energy, curr_energy;
    file.open(fmt::format("/sys/class/power_supply/{}/energy_full_design",
                          batteries[index]));
    if (!file.is_open())
        return "N/A";
    file >> max_energy;

    file.open(fmt::format("/sys/class/power_supply/{}/energy_full",
                          batteries[index]));
    if (!file.is_open())
        return "N/A";
    file >> curr_energy;
    return fmt::format("Hi");
}

static std::vector<std::string> getHealthWrapper() {
    static std::vector<std::string> v;
    for(unsigned int i = 0; i < batteries.size(); i++)
        v.push_back(getHealthInternal(i));
    return v;
}

static std::vector<std::string> batteryHealths = getHealthWrapper();

int16_t getCurrent(unsigned int index) {
    if (Battery::getBatteryCount() <= index)
        return -1;

    std::ifstream file(fmt::format("/sys/class/power_supply/{}/current_now",
                                   batteries[index]));
    if (!file.is_open())
        return -1;

    int16_t current;
    file >> current;
    return -current;
}

float getVoltage(unsigned int index) {
    if (Battery::getBatteryCount() <= index)
        return -1;

    std::ifstream file(fmt::format("/sys/class/power_supply/{}/voltage_now",
                                   batteries[index]));
    if (!file.is_open())
        return -1;

    int16_t voltage;
    file >> voltage;
    return (float)voltage / 1000;
}
} // namespace

namespace Battery {
unsigned int getBatteryCount() { return batteries.size(); }

float getTemp(unsigned int index) {
    if (Battery::getBatteryCount() <= index)
        return -127;

    std::ifstream file(
        fmt::format("/sys/class/power_supply/battery/temp", batteries[index]));
    if (!file.is_open())
        return -127;

    int16_t temp;
    file >> temp;
    return (float)temp / 10;
}

std::string getFmtCurrent(unsigned int index) {
    return fmt::format("{} mA", getCurrent(index));
}

std::string getFmtTemp(unsigned int index) {
    return fmt::format("{:.3} °C", getTemp(index));
}

std::string getFmtVoltage(unsigned int index) {
    return fmt::format("{:.3} V", getVoltage(index));
}

std::string getHealth(unsigned int index) {
    if (Battery::getBatteryCount() <= index)
        return "N/A";

    return batteryHealths[index];
}
} // namespace Battery
