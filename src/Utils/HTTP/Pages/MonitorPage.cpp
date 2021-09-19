#include "MonitorPage.h"

#include "Utils/FileSystem/FileSystem.h"
#include "Utils/Parsers/JsonParser.h"
#include "FileNames.h"
#include "Commands.h"

#include "Utils/Logger/Logger.h"

namespace Pages
{

    MonitorPage::MonitorPage(ESP8266WebServer &server) : BasePage::BasePage(server)
    {
        _HTTP->on(F("/monitor"), std::bind(&MonitorPage::Page, this));
        _HTTP->on(F("/monitor/getData"), std::bind(&MonitorPage::GetData, this));
        _HTTP->on(F("/monitor/sendCommand"), std::bind(&MonitorPage::ParseCommand, this));

        this->notSendData.clear();
    }

    void MonitorPage::Page()
    {
        _HTTP->send(200, F("text/html"), FileSystem::ReadFile(MONITOR_PAGE_PATH));
    }

    void MonitorPage::GetData()
    {
        const int dataCount = 3;

        String names[dataCount]{
            F("millis"),
            F("freeHeap"),
            F("log"),
        };

        this->notSendData.replace(String('"'), F("\\\""));

        String data[dataCount]{
            String(millis()),
            String(ESP.getFreeHeap()),
            this->notSendData,
        };

        String json = JsonParser::BuildJson(names, data, dataCount);
        this->notSendData.clear();
        _HTTP->send(200, F("text/json"), json);
    }

    void MonitorPage::AddWebLog(const String &data)
    {
        notSendData += data;
    }

    void MonitorPage::ParseCommand()
    {
        String command = _HTTP->arg(F("command"));

        if (command.indexOf(COMMAND_STOP_CHAR) == -1)
        {
            requestCommands += command;
            requestCommands += COMMAND_STOP_CHAR;
        }

        _HTTP->send(200, F("text/html"), F("Ok"));
    }

    const String MonitorPage::GetCommand()
    {
        String command;

        int indexEndCommand = requestCommands.indexOf(COMMAND_STOP_CHAR);

        if (indexEndCommand != -1)
        {
            command = requestCommands.substring(0, indexEndCommand + 1);
            requestCommands = requestCommands.substring(indexEndCommand + 1, requestCommands.length());
        }

        return command;
    }

}