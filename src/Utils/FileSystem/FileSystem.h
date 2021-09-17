#pragma once

#include <Arduino.h>
#include <LittleFS.h>

namespace FileSystem
{
    void Init();

    String ReadFile(const String &fileName);
    void WriteFile(const String &fileName, const String &data);

    bool FileExists(const String &fileName);

    File OpenFile(const String &path, const char *mode);
    Dir OpenDir(const String &path);

    bool DeleteFile(const String &path);

}