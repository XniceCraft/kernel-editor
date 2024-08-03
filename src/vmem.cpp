#include <fstream>
#include <string>

#include "ftxui/component/component.hpp"
#include "utils.hpp"
#include "vmem.hpp"
#include <fmt/core.h>

using namespace ftxui;

namespace VirtualMemoryManager {
bool getZramState() {
    std::ifstream file("/proc/swaps");
    if (!file.is_open())
        return false;

    std::string line;
    while (std::getline(file, line)) {
        if (line.find("/dev/block/zram0") != std::string::npos) {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

std::string getZramSize() {
    std::ifstream file("/sys/block/zram0/disksize");
    if (!file.is_open())
        return "N/A";

    size_t diskSize;
    file >> diskSize;
    return getHumanReadableSize(diskSize);
}

Component getTab() {
    static std::vector<std::string> toggleStr{"Disable", "Enable"};
    static Component toggle =
        Toggle(&toggleStr, &VirtualMemoryManager::toggleSelected);

    static Component vmemContainer = Container::Vertical({toggle});

    return Renderer(vmemContainer, [&] {
        return window(text("Zram") | bold,
                      vbox({hbox({text("Toggle: "), toggle->Render()}),
                            text(fmt::format("Size: {}", getZramSize()))}));
    });
}
} // namespace VirtualMemoryManager
