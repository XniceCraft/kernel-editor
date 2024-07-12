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
#include <fmt/core.h>

#include "cpu.hpp"

using namespace ftxui;

int
findInVector(const std::vector<std::string>& v, const std::string& val)
{
    for (int i = 0; i < v.size(); i++)
        if (v[i] == val)
            return i;
    return -1;
}

int
main()
{
    CpuManager cpuMgr;

    std::vector<std::string> tabMenus{
        "Overview",
        "CPU",
    };
    int tabSelected = 0;
    auto tabToggle = Toggle(&tabMenus, &tabSelected);

    // Tab: CPU
    std::vector<std::string> govs = cpuMgr.getGovernors();
    int selectedGov = findInVector(govs, cpuMgr.getGovernor());

    auto cpuFreqElement = [&] {
        Elements cpuCoreFreqInfo;
        for (unsigned int i = 0; i < cpuMgr.maximumCore(); i++)
            cpuCoreFreqInfo.push_back(text(
              fmt::format("Core {}: {:9}", i, cpuMgr.getReadableCoreFreq(i))));
        return cpuCoreFreqInfo;
    };

    auto cpuGov = Dropdown(
      { .radiobox = { .entries = &govs,
                      .selected = &selectedGov,
                      .on_change = [&] {} },
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
        } });

    auto cpuConfigContainer = Container::Vertical({
      Container::Horizontal({ cpuGov }),
    });

    auto cpuTab = Renderer(cpuConfigContainer, [&] {
        return hbox(
          { window(text("Core Freq") | bold, vbox(cpuFreqElement())) | flex,
            window(text("Configuration") | bold,
                   vbox({ hbox({ text("Governor : "), cpuGov->Render() }) })) |
              flex });
    });

    // Tab: Overview

    auto overviewTab = Renderer([&] {
        Element cpuTemp = text(cpuMgr.getReadableTemp());

        auto container = vbox({
          window(text("CPU Core Freq") | bold, vbox(cpuFreqElement())),
          window(text("CPU Temp") | bold, cpuTemp),
        });
        return container;
    });

    // Conf
    auto tabContainer = Container::Tab(
      {
        overviewTab,
        cpuTab,
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
    return 0;
}
