
#include <ESP8266WebServer.h>
#include "Utils/FileSystem/FileSystem.h"

namespace ConnectPage
{
    void Page();

    ESP8266WebServer *_HTTP;

    void Init(ESP8266WebServer &server)
    {
        _HTTP = &server;

        _HTTP->on(F("/connect"), Page);
    }

    void Page()
    {
        auto page = FileSystem::ReadFile(F("/connect.htm"));
        String ssidNew = _HTTP->arg(F("ssid"));
        String passNew = _HTTP->arg(F("pass"));

        if (ssidNew == "" || passNew == "")
        {
            String buf = "";
            int wifiCount = WiFi.scanNetworks();
            for (int i = 0; i < wifiCount; i++)
            {
                buf += F("<option>");
                buf += WiFi.SSID(i);
                buf += F("</option>");
            }
            if (wifiCount == 0)
            {
                buf = F("<option>no WiFi network</option>");
            }
            page.replace(F("|past wifi items|"), buf);
            buf = "";
        }
        else
        {
            FileSystem::WiFiConfig config;
            config.ssid = ssidNew;
            config.password = passNew;
            FileSystem::SaveWiFiData(config);
            _HTTP->send(200, F("text/html"), FileSystem::ReadFile(F("/connectSuccess.htm")));
            delay(3000);
            ESP.restart();
        }

        _HTTP->send(200, F("text/html"), page);
    }
}