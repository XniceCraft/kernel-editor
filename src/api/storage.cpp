#include "storage.hpp"
#include <fmt/core.h>
#include <string>
#include <sys/stat.h>
#include <sys/statvfs.h>

namespace {
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
} // namespace

namespace Storage {
std::string getInternalFree() {
    struct statvfs stat;
#if __ANDROID__
    if (statvfs("/sdcard", &stat) != 0)
#else
    if (statvfs("/", &stat) != 0)
#endif
        return "N/A";

    return getHumanReadableSize(stat.f_bsize * stat.f_bfree);
}

std::string getInternalTotal() {
    struct statvfs stat;
#if __ANDROID__
    if (statvfs("/sdcard", &stat) != 0)
#else
    if (statvfs("/", &stat) != 0)
#endif
        return "N/A";

    return getHumanReadableSize(stat.f_bsize * stat.f_blocks);
}

#if __ANDROID__
bool isExternalAvailable() {
    struct stat dir;
    return (stat("/storage/sdcard1", &dir) == 0) && (dir.st_mode & S_IFDIR);
}

std::string getExternalFree() {
    if (!isExternalAvailable())
        return "N/A";

    struct statvfs stat;
    if (statvfs("/storage/sdcard1", &stat) != 0)
        return "N/A";

    return getHumanReadableSize(stat.f_bsize * stat.f_bfree);
}

std::string getExternalTotal() {
    if (!isExternalAvailable())
        return "N/A";

    struct statvfs stat;
    if (statvfs("/storage/sdcard1", &stat) != 0)
        return "N/A";

    return getHumanReadableSize(stat.f_bsize * stat.f_blocks);
}
#endif
} // namespace Storage