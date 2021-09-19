#pragma once

#include <ESP8266WebServer.h>

namespace Pages
{
    class BasePage
    {
    protected:
        ESP8266WebServer *_HTTP;

    private:
        virtual void Page() = 0;

    public:
        BasePage(ESP8266WebServer &server)
        {
            _HTTP = &server;
        }

        virtual const String GetCommand()
        {
            return String();
        }
        
    };
}