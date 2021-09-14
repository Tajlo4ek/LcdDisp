#include "ConnectPage.h"

#include "Utils/FileSystem/FileSystem.h"
#include "Utils/Internet/WifiUtils.h"

namespace Pages
{
    ConnectPage::ConnectPage(ESP8266WebServer &server) : Pages::BasePage(server)
    {
        _HTTP->on(String(F("/connect")), std::bind(&ConnectPage::Page, this));
        _HTTP->on(String(F("/connect/getSSID")), std::bind(&ConnectPage::GetSSID, this));
        _HTTP->on(String(F("/connect/setWiFi")), std::bind(&ConnectPage::SetWiFi, this));
    }

    void ConnectPage::Page()
    {
        auto page = FileSystem::ReadFile(F("/connect.html"));
        _HTTP->send(200, F("text/html"), page);
    }

    void ConnectPage::GetSSID()
    {
        String json = "{\"SSIDs\":[";

        int wifiCount = WiFi.scanNetworks();
        for (int i = 0; i < wifiCount; i++)
        {
            json += "\"" + WiFi.SSID(i) + "\",";
        }

        if (json[json.length() - 1] == ',')
        {
            json.setCharAt(json.length() - 1, ']');
        }
        else
        {
            json += ']';
        }

        json += '}';

        _HTTP->send(200, F("text/json"), json);
    }

    void ConnectPage::SetWiFi()
    {
        String ssidNew = _HTTP->arg(F("ssid"));
        String passNew = _HTTP->arg(F("pass"));

        if (ssidNew.isEmpty() == false && passNew.isEmpty() == false)
        {
            WifiUtils::WiFiConfig config;
            config.ssid = ssidNew;
            config.password = passNew;
            WifiUtils::SaveWiFiConfig(config);

            _HTTP->send(200, F("text/html"), "Data save. ESP reset.");
            delay(2000);
            ESP.restart();
        }

        _HTTP->send(200, F("text/html"), "bad data");
    }

}