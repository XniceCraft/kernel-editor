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

using namespace ftxui;

namespace {
enum cpuTurboType {
    NOAVAIL,
    INTEL,
    CPUFREQ,
};

cpuTurboType getAvailTurbo() {
    std::ifstream file("/sys/devices/system/cpu/intel_pstate/no_turbo");
    if (file.good())
        return cpuTurboType::INTEL;

    file.open("/sys/devices/system/cpu/cpufreq/boost");
    if (file.good())
        return cpuTurboType::CPUFREQ;

    return cpuTurboType::NOAVAIL;
}

void apply() {
    CpuManager::dataChanged = false;
    CpuManager::setGovernor();
}

ButtonOption buttonStyle() {
    auto option = ButtonOption::Animated();
    option.transform = [](const EntryState &s) {
        auto element = text(s.label);
        if (s.focused) {
            element |= bold;
        }
        return element | center | borderEmpty | flex;
    };
    return option;
}

static float minTemp = 1000;
static float maxTemp = -1;
static cpuTurboType availableTurbo = getAvailTurbo();

bool getTurboState() {
    if (availableTurbo == cpuTurboType::NOAVAIL)
        return false;

    bool state = false;
    if (availableTurbo == cpuTurboType::INTEL) {
        std::ifstream file("/sys/devices/system/cpu/intel_pstate/no_turbo");
        file >> state;
        state = !state;
        file.close();
    } else {
        std::ifstream file("/sys/devices/system/cpu/cpufreq/boost");
        file >> state;
        file.close();
    }
    return state;
}

void setTurbo(bool val) {
    if (availableTurbo == cpuTurboType::NOAVAIL)
        return;

    if (availableTurbo == cpuTurboType::INTEL) {
        std::ofstream file("/sys/devices/system/cpu/intel_pstate/no_turbo");
        file << (val ? "0" : "1");
        return;
    }
    std::ofstream file("/sys/devices/system/cpu/cpufreq/boost");
    file << (val ? "1" : "0");
}

static std::vector<std::string> toggleEntry = {"Off", "On"};
static int turboSelectedToggle = getTurboState();
static int lastTurboSelectedToggle = turboSelectedToggle;
} // namespace

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
        if (type == "mtktscpu" || type == "x86_pkg_temp")
            return parseThermalZoneNum(entry->d_name);
    }
    closedir(dir);
    return -1;
}

std::string getFmtMaxTemp() { return fmt::format("{:.3} °C", maxTemp); }
std::string getFmtMinTemp() { return fmt::format("{:.3} °C", minTemp); }

float getTemp() {
    if (CpuManager::thermalZone == -1)
        return -127;

    std::ifstream file(fmt::format("/sys/class/thermal/thermal_zone{}/temp",
                                   CpuManager::thermalZone));
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

std::string getFmtCoreFreq(unsigned int index) {
    unsigned int freq = getCoreFreq(index);
    if (freq == 0)
        return "Off";
    return fmt::format("{} MHz", freq / 1000);
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

Component getTab() {
    static Component cpuGov = Dropdown(
        {.radiobox = {.entries = &CpuManager::governors,
                      .selected = &CpuManager::selectedGovernor,
                      .on_change = [&] { CpuManager::dataChanged = true; }},
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

    static Component applyButton =
        Maybe(Button("Apply", apply, buttonStyle()), &CpuManager::dataChanged);
    static Component turboToggle =
        Maybe(Toggle(&toggleEntry, &turboSelectedToggle),
              [&] { return getAvailTurbo() != cpuTurboType::NOAVAIL; });
    components = {cpuGov, applyButton, turboToggle};

    return Renderer(Container::Vertical(std::move(components)), [&] {
        if (lastTurboSelectedToggle != turboSelectedToggle) {
            CpuManager::dataChanged = true;
            lastTurboSelectedToggle = turboSelectedToggle;
        }

        Elements cpuCoreFreqInfo;
        for (unsigned int i = 0; i < maxCore(); i++)
            cpuCoreFreqInfo.push_back(
                text(fmt::format("Core {}: {}", i, getFmtCoreFreq(i))));

        Elements confElems = {
            vbox({hbox({text("Governor : "), cpuGov->Render()}),
                  hbox({text("Turbo Boost: "), getAvailTurbo() != cpuTurboType::NOAVAIL ? turboToggle->Render() : text("N/A")}),
                  applyButton->Render()})};

        return hbox(
            {vbox({window(text(" Core Freq ") | bold,
                          vbox(std::move(cpuCoreFreqInfo))),
                   hbox({window(text(" Min Temp ") | bold,
                                text(getFmtMinTemp())) |
                             flex,
                         window(text(" Current Temp ") | bold,
                                text(getFmtTemp())) |
                             flex,
                         window(text(" Max Temp ") | bold,
                                text(getFmtMaxTemp())) |
                             flex})}) |
                 flex,
             window(text(" Configuration ") | bold, vbox(confElems)) | flex});
    });
}
}; // namespace CpuManager
