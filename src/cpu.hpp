#pragma once

class CpuManager {
    public:
        unsigned int getCoreFreq(unsigned int index);
        std::string getReadableCoreFreq(unsigned int index);
        uint16_t maximumCore();
    private:
        uint16_t maxCore = 0;
};
