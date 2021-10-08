#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>

namespace WifiUtils
{

#define BASE_IP_STRING F("192.168.0.43")
#define BASE_SSID F("HOME")
#define BASE_PASS F("1234567890")

    typedef void (*TryConnectCallback)(int);

    struct WiFiConfig
    {
        String ssid;
        String password;
    };

    bool ConnectWifi(const String &ssid, const String &password, uint connectTries, TryConnectCallback callback);
    void StartAP(const String &ssid, const String &password);

    WiFiConfig LoadWiFiConfig();
    void SaveWiFiConfig(WiFiConfig config);

    String GetIpString();

} // namespace WifiUtils
