#include "WifiUtils.h"

#include "FileNames.h"

#include "Utils/FileSystem/FileSystem.h"
#include "Utils/Parsers/JsonParser.h"
#include "Utils/Logger/Logger.h"

namespace WifiUtils
{

#define SSID F("ssid")
#define PASSWORD F("pass")

#define DEFAULT_SSID F("no_data")
#define DEFAULT_PASSWORD F("password")

    bool ConnectWifi(const String &ssid, const String &password, uint connectTries, TryConnectCallback callback = nullptr)
    {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid.c_str(), password.c_str());
        while (--connectTries && WiFi.status() != WL_CONNECTED)
        {
            if (callback != nullptr)
            {
                callback(connectTries);
            }
            delay(1000);
        }
        return WiFi.status() == WL_CONNECTED;
    }

    void StartAP(const String &ssid, const String &password)
    {
        WiFi.disconnect();
        WiFi.mode(WIFI_AP);
        IPAddress apIP;
        apIP.fromString(BASE_IP_STRING);
        WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
        WiFi.softAP(ssid, password);
    }

    String GetIpString()
    {
        auto wifiMode = WiFi.getMode();

        if (wifiMode == WIFI_STA)
        {
            return WiFi.localIP().toString();
        }
        else if (wifiMode == WIFI_AP)
        {
            return BASE_IP_STRING;
        }
        else
        {
            return F("error");
        }
    }

    WiFiConfig LoadWiFiConfig()
    {
        WiFiConfig config;
        config.ssid = DEFAULT_SSID;
        config.password = DEFAULT_PASSWORD;

        if (FileSystem::FileExists(WIFI_CONFIG_PATH) == false)
        {
            SaveWiFiConfig(config);
            return config;
        }

        String json = FileSystem::ReadFile(WIFI_CONFIG_PATH);

        bool isOk = false;
        auto ssid = JsonParser::GetJsonData(json, SSID, isOk);
        if (isOk == false)
        {
            return config;
        }

        auto password = JsonParser::GetJsonData(json, PASSWORD, isOk);
        if (isOk == false)
        {
            return config;
        }

        config.ssid = ssid;
        config.password = password;
        return config;
    }

    void SaveWiFiConfig(WiFiConfig config)
    {
        const int dataCount = 2;
        String names[dataCount]{SSID, PASSWORD};
        String data[dataCount]{config.ssid, config.password};
        String json = JsonParser::BuildJson(names, data, dataCount);
        FileSystem::WriteFile(WIFI_CONFIG_PATH, json);
    }

} // namespace WifiUtils