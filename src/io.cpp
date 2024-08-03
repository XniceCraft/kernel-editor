#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "ftxui/component/captured_mouse.hpp" // for ftxui
#include "ftxui/component/component.hpp"      // for Input, Renderer, Vertical
#include "ftxui/component/component_base.hpp" // for ComponentBase
#include "ftxui/component/component_options.hpp" // for InputOption
#include "ftxui/component/screen_interactive.hpp" // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp" // for text, hbox, separator, Element, operator|, vbox, border
#include "io.hpp"
#include <fmt/core.h>

using namespace ftxui;

namespace IOManager {
std::string getScheduler() {
    std::ifstream file("/sys/block/mmcblk0/queue/scheduler");
    if (!file.is_open())
        return "N/A";

    std::stringstream sched;
    sched << file.rdbuf();

    std::string result = sched.str();
    int start = result.find("[");
    return result.substr(start + 1, result.find("]", start) - start - 1);
}

void setScheduler() {
    std::ofstream file("/sys/block/mmcblk0/queue/scheduler");
    if (!file.is_open())
        return;

    file << IOManager::schedulers[IOManager::selectedSched];
    file.close();
}

std::vector<std::string> getSchedulers() {
    std::vector<std::string> scheds;
    std::ifstream file("/sys/block/mmcblk0/queue/scheduler");
    if (!file.is_open())
        return scheds;

    std::stringstream fileBuffer;
    std::string buffer;
    fileBuffer << file.rdbuf();

    while (getline(fileBuffer, buffer, ' '))
        if (buffer.length() > 1) {
            if (buffer.compare(0, 1, "[") == 0) {
                buffer.erase(buffer.length() - 1);
                buffer.erase(0, 1);
            }
            scheds.push_back(buffer);
        }
    return scheds;
}

int getSchedulerIndex() {
    return std::find(IOManager::schedulers.begin(), IOManager::schedulers.end(),
                     IOManager::getScheduler()) -
           IOManager::schedulers.begin();
}

int getReadAhead() {
    std::ifstream file("/sys/block/mmcblk0/queue/read_ahead_kb");
    if (!file.is_open())
        return -1;

    int size;
    file >> size;
    return size;
}

std::string getFmtReadAhead() { return fmt::format("{} kb", getReadAhead()); }

Component getTab() {
    static Component internalSchedComp = Dropdown(
        {.radiobox = {.entries = &IOManager::schedulers,
                      .selected = &IOManager::selectedSched},
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

    static auto readAheadInputComp =
        Input(&IOManager::readAheadInput, "kilobytes");
    readAheadInputComp |= CatchEvent([&](Event event) {
        return event.is_character() && !std::isdigit(event.character()[0]);
    });

    static Component ioContainer = Container::Vertical({
        internalSchedComp,
        readAheadInputComp,
    });

    return Renderer(ioContainer, [&] {
        return window(
            text("Internal Storage") | bold,
            vbox({hbox({text("Scheduler: "), internalSchedComp->Render()}),
                  hbox({text("Read ahead: "), readAheadInputComp->Render(),
                        text("kb") | flex})}));
    });
}

} // namespace IOManager
