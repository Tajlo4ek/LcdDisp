#include "MonitorPage.h"

#include "Utils/FileSystem/FileSystem.h"
#include "Utils/Parsers/JsonParser.h"

namespace Pages
{

    MonitorPage::MonitorPage(ESP8266WebServer &server) : BasePage::BasePage(server)
    {
        _HTTP->on(F("/monitor"), std::bind(&MonitorPage::Page, this));
        _HTTP->on(F("/monitor/getData"), std::bind(&MonitorPage::GetData, this));

        this->notSendData = String();
    }

    void MonitorPage::Page()
    {
        _HTTP->send(200, F("text/html"), FileSystem::ReadFile(F("/monitor.html")));
    }

    void MonitorPage::GetData()
    {
        const int dataCount = 3;

        String names[dataCount]{
            F("millis"),
            F("freeHeap"),
            F("log"),
        };

        this->notSendData.replace(F("\""), F("\\\""));

        String data[dataCount]{
            String(millis()),
            String(ESP.getFreeHeap()),
            this->notSendData,
        };

        String json = JsonParser::BuildJson(names, data, dataCount);
        this->notSendData = String();
        _HTTP->send(200, F("text/json"), json);
    }

    void MonitorPage::AddWebLog(const String &data)
    {
        notSendData += data;
    }

}