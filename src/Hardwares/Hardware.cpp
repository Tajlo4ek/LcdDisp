#include "Hardware.h"

#include "Hardwares/CPU.h"

namespace Hardwares
{
    Hardware::Hardware(HardwareTypes hardwareType)
    {
        this->hardwareType = hardwareType;
    }

    Hardware *Hardware::FromJson(const String &json, HardwareTypes type)
    {
        switch (type)
        {
        case HardwareTypes::CPU_TYPE:
            return new CPU(json);
            break;
        case HardwareTypes::GPU_TYPE:
            break;
        case HardwareTypes::HDD_TYPE:
            break;
        case HardwareTypes::RAM_TYPE:
            break;
        }

        return nullptr;
    }

    const String Hardware::GetName() const
    {
        return this->name;
    }

    Hardware::~Hardware()
    {
        this->name.clear();
    }

}