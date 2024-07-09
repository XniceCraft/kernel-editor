#include <iostream>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include <fmt/core.h>

#include "cpu.hpp"

using namespace ftxui;

int main() {
    CpuManager cpuMgr;

    std::vector<std::string> tabMenus{
        "Overview",
        "CPU",
    };
    int tabSelected = 0;
    auto tabToggle = Toggle(&tabMenus, &tabSelected);

    std::vector<std::string> govs = cpuMgr.getGovernors();
    int selectedGov = 0;

    auto tabContainer = Container::Tab(
    {
        Renderer([&] {
            Elements cpuCoreFreqInfo;
            for(unsigned int i = 0; i < cpuMgr.maximumCore(); i++)
                cpuCoreFreqInfo.push_back(text(fmt::format("Core {}: {}", i, cpuMgr.getReadableCoreFreq(i))));
            Element cpuTemp = text(cpuMgr.getReadableTemp());

            auto container = vbox({
                window(text("CPU Core Freq") | bold, vbox(cpuCoreFreqInfo)),
                window(text("CPU Temp") | bold, cpuTemp),
            });
            return container;
        }),
        Container::Horizontal({
            Renderer([&] {
                return text(fmt::format("Governor: ", cpuMgr.getGovernor()));
            }),
            Dropdown({
                .checkbox = {
                    .label = govs[selectedGov]
                },
                .radiobox = {
                    .entries = &govs,
                    .selected = &selectedGov,
                    .on_change = []{},
                },
                .transform =
                  [](bool open, Element checkbox, Element radiobox) {
                    if (open) {
                      return vbox({
                          checkbox | inverted,
                          radiobox | vscroll_indicator | frame |
                              size(HEIGHT, LESS_THAN, 10),
                          filler(),
                      });
                    }
                    return vbox({
                        checkbox,
                        filler(),
                    });
                  },
            })
        })
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
        }) | border;
    });

    auto screen = ScreenInteractive::TerminalOutput();
    auto th = std::thread([&]() {
        while(1) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            screen.PostEvent(Event::Custom);
        }
    });
    screen.Loop(renderer);
    th.join();
    return 0;
}
