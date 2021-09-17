#pragma once

#include "BasePage.h"

namespace Pages
{
    class EditPage : public Pages::BasePage
    {
    public:
        EditPage(ESP8266WebServer &server);

    private:
        void Page() override;

        void GetFileData();
        void SaveFileData();
        void DeleteFile();

        void GetDirList();
        String GetDirJson(const String &path);
    };
}