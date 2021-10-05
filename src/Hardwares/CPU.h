#pragma once

#include "Hardware.h"

namespace Hardwares
{
    class CPU : public Hardware
    {
    public:
        struct CpuCore
        {
            int temp;
            int load;
            int clock;
        };

        void AddData(const String &json) override;

        int GetAvgLoad();
        int GetAvgTemp();

        const int GetCoreCount();
        const CpuCore *GetCores();

        CPU(const String &json);
        ~CPU();

    private:
        int coreCount;
        CpuCore *cores;
    };

}