#pragma once

#include <Arduino.h>

namespace Hardwares
{
    enum HardwareTypes
    {
        CPU_TYPE,
        HDD_TYPE,
        GPU_TYPE,
        RAM_TYPE,
    };

    class Hardware
    {
    protected:
        String name;
        HardwareTypes hardwareType;

        Hardware(HardwareTypes hardwareType);

    public:
        virtual ~Hardware();
        virtual void AddData(const String &json) = 0;

        const String GetName() const;

        static Hardware *FromJson(const String &json, HardwareTypes type);
    };
}