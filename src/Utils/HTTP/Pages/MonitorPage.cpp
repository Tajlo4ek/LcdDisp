#include "MonitorPage.h"

#include "Utils/FileSystem/FileSystem.h"
#include "Utils/Parsers/JsonParser.h"

namespace Pages
{

    MonitorPage::MonitorPage(ESP8266WebServer &server) : BasePage::BasePage(server)
    {
        _HTTP->on("/monitor", std::bind(&MonitorPage::Page, this));
        _HTTP->on("/monitor/getStats", std::bind(&MonitorPage::GetStats, this));
        _HTTP->on("/monitor/getLog", std::bind(&MonitorPage::GetWebLog, this));

        this->NotSendData = "";
    }

    void MonitorPage::Page()
    {
        _HTTP->send(200, F("text/html"), FileSystem::ReadFile(F("/monitor.html")));
    }

    void MonitorPage::GetStats()
    {
        const int dataCount = 2;

        String names[dataCount]{
            "freeHeap",
            "freeBlock",
        };

        String data[dataCount]{
            String(ESP.getFreeHeap()),
            String(ESP.getMaxFreeBlockSize()),
        };

        String json = JsonParser::BuildJson(names, data, dataCount);

        _HTTP->send(200, F("text/json"), json);
    }

    void MonitorPage::AddWebLog(String data)
    {
        NotSendData += data;
    }

    void MonitorPage::GetWebLog()
    {
        _HTTP->send(200, F("text/json"), NotSendData);
        NotSendData = "";
    }

}