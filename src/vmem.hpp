#pragma once

#include "ftxui/component/component.hpp"
#include <string>

namespace VirtualMemoryManager {
bool getZramState();
std::string getZramSize();
ftxui::Component getTab();

static int toggleSelected = VirtualMemoryManager::getZramState();
}; // namespace VirtualMemoryManager
