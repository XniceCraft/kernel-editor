#include <cctype>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/color.hpp"
#include <fmt/core.h>

#include "battery.hpp"
#include "cpu.hpp"
#include "io.hpp"
#include "vmem.hpp"

using namespace ftxui;

int findInVector(const std::vector<std::string> &v, const std::string &val) {
    for (int i = 0; i < v.size(); i++)
        if (v[i] == val)
            return i;
    return -1;
}

int main() {
    CpuManager cpuMgr;
    BatteryManager battery;
    IOManager io;
    VirtualMemoryManager vmem;

    std::vector<std::string> tabMenus{"Overview", "CPU", "I/O",
                                      "Virtual Memory"};
    int tabSelected = 0;
    auto tabToggle = Toggle(&tabMenus, &tabSelected);

    // Tab: Virtual Memory
    std::vector<std::string> zramToggleStr{"Disable", "Enable"};
    int zramToggleSelected = 0;
    Component zramToggle = Toggle(&zramToggleStr, &zramToggleSelected);

    Component vmemContainer = Container::Vertical({zramToggle});

    Component vmemTab = Renderer(vmemContainer, [&] {
        return window(
            text("Zram") | bold,
            vbox({hbox({text("Toggle: "), zramToggle->Render()}),
                  text(fmt::format("Size: {}", vmem.getZramSize()))}));
    });

    // Tab: I/O
    std::string readAheadInput;
    Component readAheadInputComp = Input(&readAheadInput, "kilobytes");
    readAheadInputComp |= CatchEvent([&](Event event) {
        return event.is_character() && !std::isdigit(event.character()[0]);
    });

    auto ioContainer = Container::Vertical({
        readAheadInputComp,
    });

    auto ioTab = Renderer(ioContainer, [&] {
        return window(
            text("Internal Storage") | bold,
            vbox({text(fmt::format("Scheduler: {}", io.getScheduler())),
                  hbox({text("Read ahead: "), readAheadInputComp->Render()})}));
    });

    // Tab: CPU
    std::vector<std::string> govs = cpuMgr.getGovernors();
    int selectedGov = findInVector(govs, cpuMgr.getGovernor());

    auto cpuFreqElement = [&] {
        Elements cpuCoreFreqInfo;
        for (unsigned int i = 0; i < cpuMgr.maximumCore(); i++)
            cpuCoreFreqInfo.push_back(text(fmt::format(
                "Core {}: {:9}", i, cpuMgr.getReadableCoreFreq(i))));
        return cpuCoreFreqInfo;
    };

    auto cpuGov = Dropdown(
        {.radiobox = {.entries = &govs,
                      .selected = &selectedGov,
                      .on_change = [&] {}},
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

    auto cpuConfigContainer = Container::Vertical({
        Container::Horizontal({cpuGov}),
    });

    auto cpuTab = Renderer(cpuConfigContainer, [&] {
        return hbox(
            {window(text("Core Freq") | bold, vbox(cpuFreqElement())) | flex,
             window(text("Configuration") | bold,
                    vbox({hbox({text("Governor : "), cpuGov->Render()})})) |
                 flex});
    });

    // Tab: Overview
    auto overviewTab = Renderer([&] {
        Elements elems;

        if (getuid() != 0) {
            Element warning = window(color(Color::Red, text("Warning!") | bold),
                                     text("This app doesn't running as root"));
            elems.insert(elems.begin(), warning);
        }
        Element battVolt = text(battery.getFmtVoltage());
        Element battCurrent = text(battery.getFmtCurrent());
        Element battHealth = text(battery.getHealth());
        Element battTemp = text(battery.getFmtTemp());

        Element batteryInfo =
            window(text("Battery") | bold,
                   hbox({
                       window(text("Voltage"), battVolt) | flex,
                       window(text("Current"), battCurrent) | flex,
                       window(text("Temp"), battTemp) | flex,
                       window(text("Health"), battHealth) | flex,
                   }));
        elems.push_back(batteryInfo);

        auto container = vbox(elems);
        return container;
    });

    // Conf
    auto tabContainer = Container::Tab(
        {
            overviewTab,
            cpuTab,
            ioTab,
            vmemTab,
        },
        &tabSelected);

    auto mainContainer = Container::Vertical({
        tabToggle,
        tabContainer,
    });

    auto renderer = Renderer(mainContainer, [&] {
        return vbox({
                   tabToggle->Render(),
                   separator(),
                   tabContainer->Render(),
               }) |
               border;
    });

    auto screen = ScreenInteractive::TerminalOutput();
    auto th = std::thread([&]() {
        while (1) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            screen.PostEvent(Event::Custom);
        }
    });
    screen.Loop(renderer);
    th.join();
    screen.Exit();
    return 0;
}
