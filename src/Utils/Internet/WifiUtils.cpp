#include "WifiUtils.h"

namespace WifiUtils
{

    bool ConnectWifi(String ssid, String password, uint connectTries, TryConnectCallback callback = nullptr)
    {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid.c_str(), password.c_str());
        while (--connectTries && WiFi.status() != WL_CONNECTED)
        {
            if (callback != nullptr)
            {
                callback(connectTries);
            }
            Serial.println(connectTries);
            delay(1000);
        }
        Serial.println(GetIpString());
        return WiFi.status() == WL_CONNECTED;
    }

    void StartAP(String ssid, String password)
    {
        WiFi.disconnect();
        WiFi.mode(WIFI_AP);
        IPAddress apIP;
        apIP.fromString(BaseIp);
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
            return BaseIp;
        }
        else
        {
            return "error";
        }
    }

} // namespace WifiUtils