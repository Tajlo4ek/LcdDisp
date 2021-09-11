#include "MonitorPage.h"

#include "Utils/FileSystem/FileSystem.h"
#include "Utils/Parsers/JsonParser.h"

namespace Pages
{

    MonitorPage::MonitorPage(ESP8266WebServer &server) : BasePage::BasePage(server)
    {
        _HTTP->on("/monitor", std::bind(&MonitorPage::Page, this));
        _HTTP->on("/monitor/getData", std::bind(&MonitorPage::GetData, this));

        this->NotSendData = "";
    }

    void MonitorPage::Page()
    {
        _HTTP->send(200, F("text/html"), FileSystem::ReadFile(F("/monitor.html")));
    }

    void MonitorPage::GetData()
    {
        const int dataCount = 3;

        String names[dataCount]{
            "millis",
            "freeHeap",
            "log",
        };

        String data[dataCount]{
            String(millis()),
            String(ESP.getFreeHeap()),
            NotSendData,
        };

        String json = JsonParser::BuildJson(names, data, dataCount);

        _HTTP->send(200, F("text/json"), json);
    }

    void MonitorPage::AddWebLog(String data)
    {
        NotSendData += data;
    }

}