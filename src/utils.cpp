#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "utils.hpp"
#include <fmt/core.h>

std::string getHumanReadableSize(double bytes) {
    const char *suffix[] = {"B", "KB", "MB", "GB", "TB"};
    uint8_t i = 0;
    for (i = 0; i < 5; i++) {
        if (bytes < 1024)
            break;
        bytes /= 1024.0;
    }
    return fmt::format("{:.3} {}", bytes, suffix[i]);
}

uint16_t parseCpuCount(const char *str) {
    // Source:
    // https://android.googlesource.com/platform/bionic/+/refs/heads/main/libc/private/get_cpu_count_from_string.h
    int cpu_count = 0;
    int last_cpu = -1;
    while (*str != '\0') {
        if (isdigit(*str)) {
            int cpu =
                static_cast<int>(strtol(str, const_cast<char **>(&str), 10));
            if (last_cpu != -1) {
                cpu_count += cpu - last_cpu;
            } else {
                cpu_count++;
            }
            last_cpu = cpu;
        } else {
            if (*str == ',') {
                last_cpu = -1;
            }
            str++;
        }
    }
    return cpu_count;
}

int8_t parseThermalZoneNum(const char *path) {
    int num = -1;
    sscanf(path, "thermal_zone%d", &num);
    return num;
}
