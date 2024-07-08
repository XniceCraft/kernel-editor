#pragma once

class CpuManager {
    public:
        unsigned int getCoreFreq(unsigned int index);
        std::string getReadableCoreFreq(unsigned int index);
        int maximumCore();
};
