#pragma once

#include <string>
#include "ftxui/component/component.hpp"

namespace VirtualMemoryManager {
bool getZramState();
std::string getZramSize();
ftxui::Component getTab();

static int toggleSelected = VirtualMemoryManager::getZramState();
};
