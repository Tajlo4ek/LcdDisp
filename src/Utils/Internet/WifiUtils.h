#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>

namespace WifiUtils
{

#define BaseIp "192.168.0.43"

    typedef void (*TryConnectCallback)(int);
    
    struct WiFiConfig
    {
        String ssid;
        String password;
    };

    bool ConnectWifi(String ssid, String password, uint connectTries, TryConnectCallback callback);
    void StartAP(String ssid, String password);

    WiFiConfig LoadWiFiConfig();
    void SaveWiFiConfig(WiFiConfig config);

    String GetIpString();

} // namespace WifiUtils
