#pragma once

#include <string>
#include <vector>

class IOManager {
  public:
    std::string getScheduler();
    std::vector<std::string> getSchedulers();
    int getReadAhead();
    std::string getFmtReadAhead();
};
