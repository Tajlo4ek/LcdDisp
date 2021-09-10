#include "MonitorPage.h"

#include "Utils/FileSystem/FileSystem.h"
#include "Utils/Parsers/JsonParser.h"

namespace Pages
{
    MonitorPage::MonitorPage(ESP8266WebServer &server) : BasePage::BasePage(server)
    {
        _HTTP->on("/monitor", std::bind(&MonitorPage::Page, this));
        _HTTP->on("/monitor/getStats", std::bind(&MonitorPage::GetStats, this));
    }

    void MonitorPage::Page()
    {
        _HTTP->send(200, F("text/html"), FileSystem::ReadFile(F("/monitor.html")));
    }

    void MonitorPage::GetStats()
    {
        String *names = new String[1]{
            "freeHeap",
        };

        String *data = new String[1]{
            String(ESP.getFreeHeap()),
        };

        String json = JsonParser::BuildJson(names, data, 1);

        _HTTP->send(200, F("text/json"), json);

        delete[] names;
        delete[] data;
    }

}