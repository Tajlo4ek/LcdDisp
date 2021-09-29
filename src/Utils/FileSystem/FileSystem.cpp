#include "FileSystem.h"

namespace FileSystem
{
    bool isInit = false;

    void Init()
    {
        LittleFS.begin();
        isInit = true;
    }

    inline void CheckInit()
    {
        if (isInit == false)
        {
            Init();
        }
    }

    String ReadFile(const String &fileName)
    {
        CheckInit();

        if (LittleFS.exists(fileName) == true)
        {
            File file = LittleFS.open(fileName, "r");
            auto data = file.readString();
            file.close();
            return data;
        }

        return String();
    }

    void WriteFile(const String &fileName, const String &data)
    {
        File file = LittleFS.open(fileName, "w");
        file.write(data.c_str());
        file.close();
    }

    const bool FileExists(const String &fileName)
    {
        CheckInit();
        return LittleFS.exists(fileName);
    }

    File OpenFile(const String &path, const char *mode)
    {
        CheckInit();
        return LittleFS.open(path, mode);
    }

    Dir OpenDir(const String &path)
    {
        CheckInit();
        return LittleFS.openDir(path);
    }

    const bool DeleteFile(const String &path)
    {
        CheckInit();
        return LittleFS.remove(path);
    }

}