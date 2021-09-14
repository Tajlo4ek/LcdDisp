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
        String json = GetDirJson("/");
        _HTTP->send(200, "text/json", json);
    }

    String EditPage::GetDirJson(String path)
    {
        String files = "[";
        String dirs = "[";

        auto dir = FileSystem::OpenDir(path);

        while (dir.next())
        {
            if (dir.isDirectory())
            {
                dirs += "{\"name\":\"" + dir.fileName() + "\",\"data\":" + GetDirJson(path + dir.fileName() + "/") + "},";
            }
            else
            {
                files += '"' + dir.fileName() + "\",";
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

        return "{\"files\":" + files + ",\"dirs\":" + dirs + "}";
    }

    void EditPage::GetFileData()
    {
        String fileName = _HTTP->arg("name");
        _HTTP->send(200, "text/html", FileSystem::ReadFile(fileName));
    }

    void EditPage::SaveFileData()
    {
        String fileName = _HTTP->arg("name");
        String fileData = _HTTP->arg("data");

        fileName.trim();

        if (fileName.isEmpty() == false && fileData.isEmpty() == false)
        {
            if (fileName.startsWith("/") && fileName.length() > 2)
            {
                FileSystem::WriteFile(fileName, fileData);
            }
        }

        _HTTP->send(200, "text/html", "Ok");
    }

    void EditPage::DeleteFile()
    {
        String fileName = _HTTP->arg("name");
        fileName.trim();

        if (fileName.isEmpty() == false)
        {
            if (fileName.startsWith("/") && fileName.length() > 2)
            {
                FileSystem::DeleteFile(fileName);
            }
        }

        _HTTP->send(200, "text/html", "Ok");
    }

}