
#include "HttpServer.h"
#include "Utils/FileSystem/FileSystem.h"
#include <ESP8266WebServer.h>

#include "Pages/EditPage.h"
#include "Pages/ConnectPage.h"
#include "Pages/MainPage.h"

namespace HttpServer
{
    ESP8266WebServer _HTTP(80);

    void Init()
    {
        EditPage::Init(_HTTP);
        ConnectPage::Init(_HTTP);
        MainPage::Init(_HTTP);

        _HTTP.begin();
    }

    void HandleServer()
    {
        _HTTP.handleClient();
    }

}