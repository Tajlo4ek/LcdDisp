
#include <ESP8266WebServer.h>
#include "Utils/FileSystem/FileSystem.h"

namespace MainPage
{
    void Page();

    ESP8266WebServer *_HTTP;

    void Init(ESP8266WebServer &server)
    {
        _HTTP = &server;

        _HTTP->on("/", Page);
    }

    void Page()
    {
        _HTTP->send(200, F("text/html"), FileSystem::ReadFile(F("/index.htm")));
    }

}