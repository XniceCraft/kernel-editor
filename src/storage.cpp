#include "storage.hpp"
#include <sys/stat.h>
#include <sys/statvfs.h>

namespace Storage {
long long getInternalFree() {
    struct statvfs stat;
    if (statvfs("/sdcard", &stat) != 0)
        return -1;

    return stat.f_bsize * stat.f_bfree;
}

long long getInternalTotal() {
    struct statvfs stat;
    if (statvfs("/sdcard", &stat) != 0)
        return -1;

    return stat.f_bsize * stat.f_blocks;
}

bool isExternalAvail() {
    struct stat dir;
    return (stat("/storage/sdcard1", &dir) == 0) && (dir.st_mode & S_IFDIR);
}

long long getExternalFree() {
    if (!isExternalAvail())
        return -1;

    struct statvfs stat;
    if (statvfs("/storage/sdcard1", &stat) != 0)
        return -1;

    return stat.f_bsize * stat.f_bfree;
}

long long getExternalTotal() {
    if (!isExternalAvail())
        return -1;

    struct statvfs stat;
    if (statvfs("/storage/sdcard1", &stat) != 0)
        return -1;

    return stat.f_bsize * stat.f_blocks;
}
}; // namespace Storage
