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

    Elements cpuCoreFreqInfo;
    for(unsigned int i = 0; i < cpuMgr.maximumCore(); i++)
        cpuCoreFreqInfo.push_back(text(fmt::format("Core %d: %s", i, cpuMgr.getReadableCoreFreq(i))));

    Element document = window(text("CPU Core Freq") | bold, vbox(std::move(cpuCoreFreqInfo)));

    auto renderer = Renderer([document] {
        return document;
    });
    auto screen = ScreenInteractive::TerminalOutput();
    screen.Loop(renderer);

    return 0;
}
