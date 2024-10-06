#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "ftxui/component/component.hpp"

namespace CpuManager {
unsigned int getCoreFreq(unsigned int index);
std::string getGovernor();
std::vector<std::string> getGovernors();
void setGovernor();
int8_t getThermalZone();
int getGovernorIndex();
std::string getFmtCoreFreq(unsigned int index);
std::string getFmtTemp();
ftxui::Component getTab();
std::string getFmtMaxTemp();
std::string getFmtMinTemp();
float getTemp();
uint16_t maxCore();

static bool dataChanged = false;
static std::vector<std::string> governors = getGovernors();
static int8_t thermalZone = getThermalZone();
static int selectedGovernor = getGovernorIndex();
static ftxui::Components components;
}; // namespace CpuManager
