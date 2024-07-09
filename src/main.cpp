#include <chrono>
#include <thread>
#include <vector>

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Radiobox, Renderer, Tab, Toggle, Vertical
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"
#include <fmt/core.h>

#include "cpu.hpp"

using namespace ftxui;

int main() {
    CpuManager cpuMgr;

    auto renderer = Renderer([&] {
        Elements cpuCoreFreqInfo;
        for(unsigned int i = 0; i < cpuMgr.maximumCore(); i++)
            cpuCoreFreqInfo.push_back(text(fmt::format("Core {}: {}", i, cpuMgr.getReadableCoreFreq(i))));

        Element document = window(text("CPU Core Freq") | bold, vbox(cpuCoreFreqInfo));
        return document;
    });
    auto screen = ScreenInteractive::TerminalOutput();

    auto th = std::thread([&]() {
        while(1) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            screen.PostEvent(Event::Custom);
        }
    });*/
    screen.Loop(renderer);
    th.join();
    return 0;
}
