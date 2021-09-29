#pragma once

#include "BasePage.h"

namespace Pages
{
    class MonitorPage : public Pages::BasePage
    {
    public:
        MonitorPage(ESP8266WebServer &server);

        void AddWebLog(const String &data);

        const String GetCommand() override;

    private:
        void Page() override;

        void GetData();
        void ParseCommand();

        String notSendData;
        String requestCommands;
    };
}