#pragma once

#include <string>

uint16_t parseCpuCount(const char *str);
int8_t parseThermalZoneNum(const char *path);
std::string getHumanReadableSize(double bytes);
