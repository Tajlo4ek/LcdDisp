#include "FileSystem.h"

namespace FileSystem
{
    void clearString(String &str);

#define WIFI_CONFIG_FILE "/wifiConfig.txt"
#define DATA_SPLIT_CHAR '\n'

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

    WiFiConfig GetWiFiData()
    {
        CheckInit();

        WiFiConfig config;

        if (FileExists(WIFI_CONFIG_FILE) == false)
        {
            config.ssid = "no_data";
            config.password = "";
            return config;
        }

        File file = LittleFS.open(WIFI_CONFIG_FILE, "r");

        auto ssid = file.readStringUntil(DATA_SPLIT_CHAR);
        clearString(ssid);

        auto password = file.readStringUntil(DATA_SPLIT_CHAR);
        clearString(password);

        config.ssid = ssid;
        config.password = password;

        file.close();

        return config;
    }

    void SaveWiFiData(WiFiConfig config)
    {
        CheckInit();

        File file = LittleFS.open(WIFI_CONFIG_FILE, "w");

        auto ssid = config.ssid;
        auto password = config.password;

        clearString(ssid);
        clearString(password);

        file.write(ssid.c_str());
        file.write(DATA_SPLIT_CHAR);
        file.write(password.c_str());
        file.write(DATA_SPLIT_CHAR);

        file.close();
    }

    void clearString(String &str)
    {
        str.replace("\n", "");
        str.replace("\r", "");
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