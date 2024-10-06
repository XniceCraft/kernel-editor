#include <cctype>
#include <chrono>
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
#include "storage.hpp"
#include "utils.hpp"
#include "vmem.hpp"

using namespace ftxui;

int main() {
    std::vector<std::string> tabMenus{"Overview", "CPU", "I/O",
                                      "Virtual Memory"};
    int tabSelected = 0;
    auto tabToggle = Toggle(&tabMenus, &tabSelected);

    // Tab: Overview
    auto overviewTab = Renderer([&] {
        Elements storages = {
            hbox({text("Internal "), separator(),
                  text(fmt::format(
                      " Total: {} Free: {}",
                      getHumanReadableSize(Storage::getInternalTotal()),
                      getHumanReadableSize(Storage::getInternalFree())))})};
        if (Storage::isExternalAvail()) {
            storages.push_back(separator());
            storages.push_back(
                hbox({text("External "), separator(),
                      text(fmt::format(
                          " Total: {} Free: {}",
                          getHumanReadableSize(Storage::getExternalTotal()),
                          getHumanReadableSize(Storage::getExternalFree())))}));
        }

        Element battVolt = text(Battery::getFmtVoltage()) | hcenter;
        Element battCurrent = text(Battery::getFmtCurrent()) | hcenter;
        Element battHealth = text(Battery::getHealth()) | hcenter;
        Element battTemp = text(Battery::getFmtTemp()) | hcenter;

        Elements elems = {
            window(text(" Storage ") | bold, vbox(storages)),
            window(text(" Battery ") | bold,
                   hbox({
                       window(text("Voltage") | hcenter, battVolt) | flex,
                       window(text("Current") | hcenter, battCurrent) | flex,
                       window(text("Temp") | hcenter, battTemp) | flex,
                       window(text("Health") | hcenter, battHealth) | flex,
                   }))};

        if (getuid() != 0) {
            Element warning =
                window(color(Color::Red, text(" Warning! ") | bold),
                       text("This app doesn't running as root"));
            elems.insert(elems.begin(), warning);
        }
        return vbox(elems);
    });

    // Conf
    auto tabContainer = Container::Tab(
        {
            overviewTab,
            CpuManager::getTab(),
            IOManager::getTab(),
            VirtualMemoryManager::getTab(),
        },
        &tabSelected);

    auto mainContainer = Container::Vertical({
        tabToggle,
        tabContainer,
    });

    auto renderer = Renderer(mainContainer, [&] {
        return vbox({
                   text("Kernel Editor") | hcenter,
                   separator(),
                   tabToggle->Render(),
                   separator(),
                   tabContainer->Render(),
               }) |
               border;
    });

    auto screen = ScreenInteractive::Fullscreen();
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
