#include "EditPage.h"

#include "Utils/FileSystem/FileSystem.h"
#include "Utils/Internet/WifiUtils.h"
#include "Utils/Logger/Logger.h"

namespace Pages
{
    EditPage::EditPage(ESP8266WebServer &server) : Pages::BasePage(server)
    {
        _HTTP->on(F("/edit"), std::bind(&EditPage::Page, this));
        _HTTP->on(F("/edit/getFiles"), std::bind(&EditPage::GetDirList, this));
        _HTTP->on(F("/edit/getFileData"), std::bind(&EditPage::GetFileData, this));
        _HTTP->on(F("/edit/saveFileData"), std::bind(&EditPage::SaveFileData, this));
        _HTTP->on(F("/edit/deleteFile"), std::bind(&EditPage::DeleteFile, this));
    }

    void EditPage::Page()
    {
        _HTTP->send(200, "text/html", FileSystem::ReadFile(F("edit.html")));
    }

    void EditPage::GetDirList()
    {
        String json = GetDirJson(F("/"));
        _HTTP->send(200, F("text/json"), json);
    }

    String EditPage::GetDirJson(const String &path)
    {
        String files = String(F("["));
        String dirs = String(F("["));

        auto dir = FileSystem::OpenDir(path);

        while (dir.next())
        {
            if (dir.isDirectory())
            {
                dirs += String(F("{\"name\":\""));
                dirs += dir.fileName();
                dirs += String(F("\",\"data\":"));
                dirs += GetDirJson(path + dir.fileName() + String(F("/")));
                dirs += String(F("},"));
            }
            else
            {
                files += '"';
                files += dir.fileName();
                files += String(F("\","));
            }
        }

        if (files[files.length() - 1] == ',')
        {
            files.setCharAt(files.length() - 1, ']');
        }
        else
        {
            files += ']';
        }

        if (dirs[dirs.length() - 1] == ',')
        {
            dirs.setCharAt(dirs.length() - 1, ']');
        }
        else
        {
            dirs += ']';
        }

        String res = String(F("{\"files\":"));
        res += files;
        res += String(F(",\"dirs\":"));
        res += dirs;
        res += '}';

        return res;
    }

    void EditPage::GetFileData()
    {
        String fileName = _HTTP->arg(F("name"));
        _HTTP->send(200, F("text/html"), FileSystem::ReadFile(fileName));
    }

    void EditPage::SaveFileData()
    {
        String fileName = _HTTP->arg(F("name"));
        String fileData = _HTTP->arg(F("data"));

        fileName.trim();

        if (fileName.isEmpty() == false && fileData.isEmpty() == false)
        {
            if (fileName.startsWith(F("/")) && fileName.length() > 2)
            {
                FileSystem::WriteFile(fileName, fileData);
            }
        }

        _HTTP->send(200, F("text/html"), F("Ok"));
    }

    void EditPage::DeleteFile()
    {
        String fileName = _HTTP->arg(F("name"));
        fileName.trim();

        if (fileName.isEmpty() == false)
        {
            if (fileName.startsWith(F("/")) && fileName.length() > 2)
            {
                FileSystem::DeleteFile(fileName);
            }
        }

        _HTTP->send(200, F("text/html"), F("Ok"));
    }

}