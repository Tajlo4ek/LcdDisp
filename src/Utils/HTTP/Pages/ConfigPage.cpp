#include "ConfigPage.h"

#include "Utils/FileSystem/FileSystem.h"
#include "FileNames.h"

namespace Pages
{
    ConfigPage::ConfigPage(ESP8266WebServer &server) : BasePage::BasePage(server)
    {
        _HTTP->on(F("/editConfig"), std::bind(&ConfigPage::Page, this));
    }

    void ConfigPage::Page()
    {
        String page = FileSystem::ReadFile(CONFIG_PAGE_PATH);

        page.replace(F("{clock_config}"), DIGITAL_CLOCK_CONFIG_PATH);
        page.replace(F("{spectrum_config}"), SPECTRUM_CONFIG_PATH);

        _HTTP->send(200, F("text/html"), page);
    }

}