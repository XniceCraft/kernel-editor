#include <fstream>
#include <string>
#include <vector>

#include "battery.hpp"
#include <fmt/core.h>

namespace Battery {
int16_t getCurrent() {
    std::ifstream file("/sys/class/power_supply/battery/current_now");
    if (!file.is_open())
        return -127;

    int16_t current;
    file >> current;
    return -current;
}

float getTemp() {
    std::ifstream file("/sys/class/power_supply/battery/temp");
    if (!file.is_open())
        return -127;

    int16_t temp;
    file >> temp;
    return (float)temp / 10;
}

float getVoltage() {
    std::ifstream file("/sys/class/power_supply/battery/voltage_now");
    if (!file.is_open())
        return -127;

    int16_t temp;
    file >> temp;
    return (float)temp / 1000;
}

std::string getFmtCurrent() { return fmt::format("{} mA", getCurrent()); }

std::string getFmtTemp() { return fmt::format("{:.3} °C", getTemp()); }

std::string getFmtVoltage() { return fmt::format("{:.3} V", getVoltage()); }

std::string getHealth() {
    std::ifstream file("/sys/class/power_supply/battery/health");
    if (!file.is_open())
        return "N/A";

    std::string health;
    file >> health;
    return health;
}
} // namespace Battery
