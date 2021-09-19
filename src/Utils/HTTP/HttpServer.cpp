
#include "HttpServer.h"
#include "Utils/FileSystem/FileSystem.h"
#include <ESP8266WebServer.h>

#include "Pages/ConnectPage.h"
#include "Pages/MainPage.h"
#include "Pages/MonitorPage.h"
#include "Pages/EditPage.h"
#include "Pages/ConfigPage.h"

namespace HttpServer
{
    ESP8266WebServer _HTTP(80);

    Pages::ConnectPage connectPage(_HTTP);
    Pages::MainPage mainPage(_HTTP);
    Pages::MonitorPage monitorPage(_HTTP);
    Pages::EditPage editPage(_HTTP);
    Pages::ConfigPage configPage(_HTTP);

    GetCommandCallback getCommandCallback;

    void Init(GetCommandCallback callback)
    {
        _HTTP.begin();
        getCommandCallback = callback;
    }

    void HandleServer()
    {
        _HTTP.handleClient();
        String command = monitorPage.GetCommand();

        if (command.isEmpty() == false)
        {
            getCommandCallback(command);
        }
    }

    void AddWebLog(const String &data)
    {
        monitorPage.AddWebLog(data);
    }

}