
#include "HttpServer.h"
#include "Utils/FileSystem/FileSystem.h"
#include <ESP8266WebServer.h>

#include "Pages/ConnectPage.h"
#include "Pages/MainPage.h"

namespace HttpServer
{
    ESP8266WebServer _HTTP(80);

    Pages::ConnectPage connectPage(_HTTP);
    Pages::MainPage mainPage(_HTTP);

    void Init()
    {
        _HTTP.begin();
    }

    void HandleServer()
    {
        _HTTP.handleClient();
    }

}