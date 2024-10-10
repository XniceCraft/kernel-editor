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

#include "api/battery.hpp"
#include "api/storage.hpp"

using namespace ftxui;

int main() {
    std::vector<std::string> tabMenus{"Overview", "CPU"};
    int tabSelected = 0;
    auto tabToggle = Toggle(&tabMenus, &tabSelected);

    // Tab: Overview
    auto overviewTab = Renderer([&] {
        Elements storages = {
            hbox({text("Internal "), separator(),
                  text(fmt::format(" Total: {} Free: {}",
                                   Storage::getInternalTotal(),
                                   Storage::getInternalFree()))})};
#if __ANDROID__
        if (Storage::isExternalAvail()) {
            storages.push_back(separator());
            storages.push_back(
                hbox({text("External "), separator(),
                      text(fmt::format(" Total: {} Free: {}",
                                       Storage::getExternalTotal(),
                                       Storage::getExternalFree()))}));
        }
#endif

        Elements batteries = {};
        for (unsigned int i = 0; i < Battery::getBatteryCount(); i++) {
            batteries.push_back(hbox({
                vbox({text("Voltage"), text(Battery::getFmtVoltage(i))}),
                separator(),
                vbox({text("Current"), text(Battery::getFmtCurrent(i))}),
                separator(),
                vbox({text("Temp"), text(Battery::getFmtTemp(i))}),
                separator(),
                vbox({text("Health"), text(Battery::getHealth(i))}),
            }));
            batteries.push_back(separator());
        }

        Elements elems = {window(text(" Storage ") | bold, vbox(storages)),
                          window(text(" Batteries ") | bold, vbox(batteries))};

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

    auto screen = ScreenInteractive::FitComponent();
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
