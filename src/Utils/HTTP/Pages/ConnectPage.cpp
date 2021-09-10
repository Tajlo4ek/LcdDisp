#include "ConnectPage.h"

#include "Utils/FileSystem/FileSystem.h"
#include "Utils/Internet/WifiUtils.h"

namespace Pages
{
    ConnectPage::ConnectPage(ESP8266WebServer &server) : Pages::BasePage(server)
    {
        _HTTP->on(String(F("/connect")), std::bind(&ConnectPage::Page, this));
    }

    void ConnectPage::Page()
    {
        auto page = FileSystem::ReadFile(F("/connect.html"));
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
            WifiUtils::WiFiConfig config;
            config.ssid = ssidNew;
            config.password = passNew;
            WifiUtils::SaveWiFiConfig(config);
            
            //TODO: send connect ok
            delay(3000);
            ESP.restart();
        }

        _HTTP->send(200, F("text/html"), page);
    }
}