#pragma once

#include "BasePage.h"

namespace Pages
{
    class MonitorPage : public Pages::BasePage
    {
    public:
        MonitorPage(ESP8266WebServer &server);

        void AddWebLog(const String &data);

    private:
        void Page() override;

        void GetData();

        String notSendData;
    };
}