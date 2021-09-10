#pragma once

#include "BasePage.h"

namespace Pages
{
    class ConnectPage : public Pages::BasePage
    {
    public:
        ConnectPage(ESP8266WebServer &server);

    private:
        void Page() override;
    };
}