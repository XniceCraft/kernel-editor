#include <fstream>
#include <string>
#include <vector>

#include "battery.hpp"
#include <fmt/core.h>

int16_t BatteryManager::getCurrent() {
    std::ifstream file("/sys/class/power_supply/battery/current_now");
    if (!file.is_open())
        return -127;

    int16_t current;
    file >> current;
    return -current;
}

float BatteryManager::getTemp() {
    std::ifstream file("/sys/class/power_supply/battery/temp");
    if (!file.is_open())
        return -127;

    int16_t temp;
    file >> temp;
    return (float)temp / 10;
}

float BatteryManager::getVoltage() {
    std::ifstream file("/sys/class/power_supply/battery/voltage_now");
    if (!file.is_open())
        return -127;

    int16_t temp;
    file >> temp;
    return (float)temp / 1000;
}

std::string BatteryManager::getFmtCurrent() {
    return fmt::format("{} mA", this->getCurrent());
}

std::string BatteryManager::getFmtTemp() {
    return fmt::format("{:.3} °C", this->getTemp());
}

std::string BatteryManager::getFmtVoltage() {
    return fmt::format("{:.3} V", this->getVoltage());
}

std::string BatteryManager::getHealth() {
    std::ifstream file("/sys/class/power_supply/battery/health");
    if (!file.is_open())
        return "N/A";

    std::string health;
    file >> health;
    return health;
}
