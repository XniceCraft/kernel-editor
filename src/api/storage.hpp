#pragma once

#include <string>

namespace Storage {
std::string getInternalFree();
std::string getInternalTotal();

#if __ANDROID__
bool isExternalAvailable();
std::string getExternalFree();
std::string getExternalTotal();
#endif
} // namespace Storage