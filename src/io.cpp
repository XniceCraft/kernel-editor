#include <fstream>
#include <sstream>
#include <string>

#include "io.hpp"
#include <fmt/core.h>

std::string IOManager::getScheduler() {
    std::ifstream file("/sys/block/mmcblk0/queue/scheduler");
    if (!file.is_open())
        return "N/A";

    std::stringstream sched;
    sched << file.rdbuf();

    std::string result = sched.str();
    int start = result.find("[");
    return result.substr(start + 1, result.find("]", start) - start - 1);
}

std::vector<std::string> IOManager::getSchedulers() {
    std::vector<std::string> scheds;
    return scheds;
    /*std::ifstream file("/sys/block/mmcblk0/queue/scheduler");
    if (!file.is_open())
        return "N/A";

    std::stringstream sched;
    sched << file.rdbuf();*/
}

int IOManager::getReadAhead() {
    std::ifstream file("/sys/block/mmcblk0/queue/scheduler");
    if (!file.is_open())
        return -1;

    int size;
    file >> size;
    return size;
}

std::string IOManager::getFmtReadAhead() {
    return fmt::format("{} kb", this->getReadAhead());
}
