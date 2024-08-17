#include <algorithm>
#include <cstdint>
#include <dirent.h>
#include <fstream>
#include <memory>
#include <sstream>
#include <string.h>
#include <string>
#include <vector>

#include "cpu.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/dom/elements.hpp"
#include "utils.hpp"
#include <fmt/core.h>

#include <iostream>

using namespace ftxui;

namespace CpuManager {
unsigned int getCoreFreq(unsigned int index) {
    std::ifstream file(fmt::format(
        "/sys/devices/system/cpu/cpu{}/cpufreq/scaling_cur_freq", index));
    if (!file.is_open())
        return 0;

    unsigned int freq;
    file >> freq;
    return freq;
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
        if (type == "mtktscpu")
            return parseThermalZoneNum(entry->d_name);
    }
    closedir(dir);
    return -1;
}

float getTemp() {
    if (CpuManager::thermalZone == -1)
        return -127;

    std::ifstream file(fmt::format("/sys/class/thermal/thermal_zone{}/temp",
                                   CpuManager::thermalZone));
    if (!file.is_open())
        return -127;

    int32_t temp;
    file >> temp;
    return (float)temp / 1000;
}

std::string getFmtCoreFreq(unsigned int index) {
    unsigned int freq = getCoreFreq(index);
    if (freq == 0)
        return "Off";
    return fmt::format("{:.3} GHz", (float)freq / 1000000);
}

std::string getFmtTemp() { return fmt::format("{:.3} °C", getTemp()); }

std::string getGovernor() {
    std::ifstream file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
    if (!file.is_open())
        return "N/A";

    std::string governor;
    file >> governor;
    return governor;
}

std::vector<std::string> getGovernors() {
    std::vector<std::string> govs;
    std::ifstream file(
        "/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors");
    if (!file.is_open()) {
        govs.push_back("N/A");
        return govs;
    }

    std::stringstream governor;
    std::string buffer;
    governor << file.rdbuf();

    while (getline(governor, buffer, ' '))
        if (buffer.length() > 1)
            govs.push_back(buffer);

    if (govs.empty())
        govs.push_back("N/A");
    return govs;
}

void setGovernor() {
    std::ofstream file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
    if (!file.is_open())
        return;

    file << CpuManager::governors[CpuManager::selectedGovernor];
    file.close();
}

int getGovernorIndex() {
    return std::find(CpuManager::governors.begin(), CpuManager::governors.end(),
                     getGovernor()) -
           CpuManager::governors.begin();
}

uint16_t maxCore() {
    static int16_t maxCoreCount = -1;
    if (maxCoreCount != -1)
        return maxCoreCount;

    std::ifstream file("/sys/devices/system/cpu/present");
    if (!file.is_open())
        return 0;

    std::string str;
    file >> str;
    file.close();
    maxCoreCount = parseCpuCount(str.c_str());
    return maxCoreCount;
}

void onChange() {
    CpuManager::dataChanged = false;
    setGovernor();
}

Component getTab() {
    static Component cpuGov = Dropdown(
        {.radiobox = {.entries = &CpuManager::governors,
                      .selected = &CpuManager::selectedGovernor,
                      .on_change = [&] {
                        CpuManager::dataChanged = true;
                      }},
         .transform = [](bool open, Element checkbox, Element radiobox) {
             if (open)
                 return vbox({
                     checkbox | inverted,
                     radiobox | vscroll_indicator | frame |
                         size(HEIGHT, LESS_THAN, 10),
                     filler(),
                 });
             return vbox({
                 checkbox,
                 filler(),
             });
         }});

    static Component applyButton = Button("Apply", onChange);

    components = {
        cpuGov
    };

    auto container = Container::Vertical(std::move(components));

    if (CpuManager::dataChanged) {
        components.push_back(applyButton);
    }

    return Renderer(container, [&] {
        Elements cpuCoreFreqInfo;
        for (unsigned int i = 0; i < maxCore(); i++)
            cpuCoreFreqInfo.push_back(
                text(fmt::format("Core {}: {:9}", i, getFmtCoreFreq(i))));

        Elements confElems = {
            hbox({text("Governor : "), cpuGov->Render()})
        };

        if (CpuManager::dataChanged) {
            confElems.push_back(applyButton->Render());
//            components.push_back(applyButton);
        }

        return hbox(
            {vbox({window(text(" Core Freq ") | bold,
                          vbox(std::move(cpuCoreFreqInfo))),
                   window(text(" Temperature ") | bold, text(getFmtTemp()))}) |
                 flex,
             window(text(" Configuration ") | bold,
                    vbox(confElems)) |
                 flex});
    });
}
}; // namespace CpuManager
