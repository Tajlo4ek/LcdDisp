#include "ConfigPage.h"

#include "Utils/FileSystem/FileSystem.h"
#include "FileNames.h"
#include "Commands.h"

#include "Utils/Parsers/JsonParser.h"

namespace Pages
{

    ConfigPage::ConfigPage(ESP8266WebServer &server) : BasePage::BasePage(server)
    {
        _HTTP->on(F("/editConfig"), std::bind(&ConfigPage::Page, this));
        _HTTP->on(F("/editConfig/getConfig"), std::bind(&ConfigPage::GetConfig, this));
    }

    void ConfigPage::Page()
    {
        String page = FileSystem::ReadFile(CONFIG_PAGE_PATH);

        page.replace(F("{reload-command}"), COMMAND_RELOAD_SCREEN);

        _HTTP->send(200, F("text/html"), page);
    }

    void ConfigPage::GetConfig()
    {
        const int namesCount = 3;
        String names[namesCount] = {
            F("name"),
            F("fileName"),
            F("type"),
        };

        const int configCount = 2;

        String configName[configCount] = {
            F("main screen"),
            F("spectrum screen"),
        };

        String fileNames[configCount] = {
            MAIN_SCREEN_CONFIG_PATH,
            SPECTRUM_CONFIG_PATH,
        };

        String configType[configCount] = {
            F("color"),
            F("color"),
        };

        String json = "[";

        for (int i = 0; i < configCount; i++)
        {
            String buf[namesCount] = {
                configName[i],
                fileNames[i],
                configType[i],
            };

            json += JsonParser::BuildJson(names, buf, namesCount);
            json += ',';
        }

        json.remove(json.length() - 1);
        json += ']';

        _HTTP->send(200, F("text/json"), json);
    }
}