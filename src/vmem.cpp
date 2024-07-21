#include <fstream>
#include <string>

#include "vmem.hpp"
#include <fmt/core.h>

namespace {
std::string getHumanReadableSize(size_t bytes) {
    const char *suffix[] = {"B", "KB", "MB", "GB", "TB"};
    uint8_t i = 0;
    for (i = 0; i < 5; i++) {
        if (bytes < 1024)
            break;
        bytes >>= 10;
    }
    return fmt::format("{}{}", bytes, suffix[i]);
}
} // namespace

std::string VirtualMemoryManager::getZramSize() {
    std::ifstream file("/sys/block/zram0/disksize");
    if (!file.is_open())
        return "N/A";

    size_t diskSize;
    file >> diskSize;
    return getHumanReadableSize(diskSize);
}
