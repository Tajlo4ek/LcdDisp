#pragma once

#include "BasePage.h"

namespace Pages
{
    class MonitorPage : public Pages::BasePage
    {
    public:
        MonitorPage(ESP8266WebServer &server);

        void AddWebLog(String data);

    private:
        void Page() override;

        void GetStats();
        void GetWebLog();

        String NotSendData;
    };
}