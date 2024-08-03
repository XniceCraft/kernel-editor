#pragma once

#include "ftxui/component/component.hpp"
#include <string>
#include <vector>

namespace IOManager {
std::string getScheduler();
std::vector<std::string> getSchedulers();
void setScheduler();
int getReadAhead();
int getSchedulerIndex();
std::string getFmtReadAhead();
ftxui::Component getTab();

static std::vector<std::string> schedulers = getSchedulers();
static int selectedSched = getSchedulerIndex();
static std::string readAheadInput = std::to_string(getReadAhead());
}; // namespace IOManager
