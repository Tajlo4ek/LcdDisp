#include "CPU.h"

#include "Utils/Parsers/JsonParser.h"
#include "Utils/Logger/Logger.h"

namespace Hardwares
{

    CPU::CPU(const String &json) : Hardware(HardwareTypes::CPU_TYPE)
    {
        this->name = JsonParser::GetJsonData(json, "name");

        this->coreCount = JsonParser::GetJsonData(json, "coreCount").toInt();
        this->cores = new CpuCore[this->coreCount];

        AddData(json);
    }

    void CPU::AddData(const String &json)
    {
        String coresJson = JsonParser::GetJsonData(json, "cores");
        String *coreDatas = new String[this->coreCount];
        JsonParser::ParseJsonArray(coresJson, this->coreCount, coreDatas);

        for (int coreNum = 0; coreNum < this->coreCount; coreNum++)
        {
            this->cores[coreNum].temp = JsonParser::GetJsonData(coreDatas[coreNum], "temp").toInt();
            this->cores[coreNum].load = JsonParser::GetJsonData(coreDatas[coreNum], "load").toInt();
            this->cores[coreNum].clock = JsonParser::GetJsonData(coreDatas[coreNum], "clock").toInt();
        }
    }

    CPU::~CPU()
    {
        delete[] this->cores;
    }

}