#pragma once

#include <Arduino.h>
#include <LittleFS.h>

namespace FileSystem
{
    const auto WifiConfigFileName = "/WifiConfig.json";
    const auto WeatherConfigFileName = "/WeatherConfig.json";

    void Init();

    String ReadFile(String fileName);
    void WriteFile(String fileName, String data);

    bool FileExists(String fileName);

    File OpenFile(String path, const char *mode);
    Dir OpenDir(String path);

    bool DeleteFile(String path);

}