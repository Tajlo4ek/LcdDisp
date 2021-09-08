#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>

namespace WifiUtils
{

#define BaseIp "192.168.0.43"

    typedef void (*TryConnectCallback)(int);

    bool ConnectWifi(String ssid, String password, uint connectTries, TryConnectCallback callback);
    void StartAP(String ssid, String password);

    String GetIpString();

} // namespace WifiUtils
