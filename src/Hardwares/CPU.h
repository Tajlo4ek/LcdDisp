#pragma once

#include "Hardware.h"

namespace Hardwares
{
    class CPU : public Hardware
    {

    private:
        struct CpuCore
        {
            int temp;
            int load;
            int clock;
        };

        int coreCount;
        CpuCore *cores;

    public:
        void AddData(const String &json) override;

        CPU(const String &json);
        ~CPU() override;
    };

}