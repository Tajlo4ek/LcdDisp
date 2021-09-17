#include "MainPage.h"

#include "Utils/FileSystem/FileSystem.h"

namespace Pages
{
    MainPage::MainPage(ESP8266WebServer &server) : BasePage::BasePage(server)
    {
        _HTTP->on(F("/"), std::bind(&MainPage::Page, this));
    }

    void MainPage::Page()
    {
        _HTTP->send(200, F("text/html"), FileSystem::ReadFile(F("/index.html")));
    }

}