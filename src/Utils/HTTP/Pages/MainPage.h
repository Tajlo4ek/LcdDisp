#pragma once

#include "BasePage.h"

namespace Pages
{
    class MainPage : public Pages::BasePage
    {
    public:
        MainPage(ESP8266WebServer &server);

    private:
        void Page() override;
    };
}