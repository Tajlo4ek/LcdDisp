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

    String ReadFile(String fileName)
    {
        CheckInit();

        if (LittleFS.exists(fileName) == true)
        {
            File file = LittleFS.open(fileName, "r");
            auto data = file.readString();
            file.close();
            return data;
        }

        return "";
    }

    void WriteFile(String fileName, String data)
    {
        File file = LittleFS.open(fileName, "w");
        file.write(data.c_str());
        file.close();
    }

    bool FileExists(String fileName)
    {
        CheckInit();
        return LittleFS.exists(fileName);
    }

    File OpenFile(String path, const char *mode)
    {
        CheckInit();
        return LittleFS.open(path, mode);
    }

    Dir OpenDir(String path)
    {
        CheckInit();
        return LittleFS.openDir(path);
    }

    bool Remove(String path)
    {
        CheckInit();
        return LittleFS.remove(path);
    }

}