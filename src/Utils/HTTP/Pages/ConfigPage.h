#pragma once

#include "BasePage.h"

namespace Pages
{
    class ConfigPage : public Pages::BasePage
    {
    public:
        ConfigPage(ESP8266WebServer &server);

    private:
        void Page() override;
        void GetConfig();
    };
}