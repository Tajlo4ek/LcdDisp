#pragma once

#include <Arduino.h>
#include <LittleFS.h>

namespace FileSystem
{
    struct WiFiConfig
    {
        String ssid;
        String password;
    };

    void Init();

    WiFiConfig GetWiFiData();

    void SaveWiFiData(WiFiConfig config);

    String ReadFile(String fileName);

    bool FileExists(String fileName);

    File OpenFile(String path, const char *mode);

    Dir OpenDir(String path);

    bool Remove(String path); 

}