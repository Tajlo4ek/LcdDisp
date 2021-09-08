
#include "EditPage.h"
#include "Utils/FileSystem/FileSystem.h"

namespace EditPage
{
    ESP8266WebServer *_HTTP;
    File fsUploadFile;

    bool isEditableFile(String path);
    void handleFileList();
    void handleFileCreate();
    void handleFileUpload();
    void handleFileDelete();
    bool handleFileRead(String path);
    String formatBytes(size_t bytes);

    String getContentType(String filename);

    void Init(ESP8266WebServer &server)
    {
        _HTTP = &server;

        _HTTP->on(String(F("/list")), HTTP_GET, handleFileList);
        _HTTP->on(String(F("/edit")), HTTP_GET, []() {
            if (!handleFileRead(String(F("/edit.htm"))))
            {
                _HTTP->send(404, String(F("text/plain")), String(F("FileNotFound")));
            }
        });

        _HTTP->on(String(F("/edit")), HTTP_PUT, handleFileCreate);
        _HTTP->on(String(F("/edit")), HTTP_DELETE, handleFileDelete);
        _HTTP->on(
            String(F("/edit")), HTTP_POST, []() {
                _HTTP->send(200, String(F("text/plain")), "");
            },
            handleFileUpload);

        _HTTP->onNotFound([]() {
            if (!handleFileRead(_HTTP->uri()))
            {
                _HTTP->send(404, String(F("text/plain")), String(F("FileNotFound")));
            }
        });
    }

    String formatBytes(size_t bytes)
    {
        if (bytes < 1024)
        {
            return String(bytes) + "B";
        }
        else if (bytes < (1024 * 1024))
        {
            return String(bytes / 1024.0) + String(F("KB"));
        }
        else if (bytes < (1024 * 1024 * 1024))
        {
            return String(bytes / 1024.0 / 1024.0) + String(F("MB"));
        }
        else
        {
            return String(bytes / 1024.0 / 1024.0 / 1024.0) + String(F("GB"));
        }
    }

    String getContentType(String filename)
    {
        if (_HTTP->hasArg(String(F("download"))))
        {
            return String(F("application/octet-stream"));
        }
        else if (filename.endsWith(String(F(".htm"))))
        {
            return String(F("text/html"));
        }
        else if (filename.endsWith(String(F(".html"))))
        {
            return String(F("text/html"));
        }
        else if (filename.endsWith(String(F(".css"))))
        {
            return String(F("text/css"));
        }
        else if (filename.endsWith(String(F(".js"))))
        {
            return String(F("application/javascript"));
        }
        else if (filename.endsWith(String(F(".png"))))
        {
            return String(F("image/png"));
        }
        else if (filename.endsWith(String(F(".gif"))))
        {
            return String(F("image/gif"));
        }
        else if (filename.endsWith(String(F(".jpg"))))
        {
            return String(F("image/jpeg"));
        }
        else if (filename.endsWith(String(F(".ico"))))
        {
            return String(F("image/x-icon"));
        }
        else if (filename.endsWith(String(F(".xml"))))
        {
            return String(F("text/xml"));
        }
        else if (filename.endsWith(String(F(".pdf"))))
        {
            return String(F("application/x-pdf"));
        }
        else if (filename.endsWith(String(F(".zip"))))
        {
            return String(F("application/x-zip"));
        }
        else if (filename.endsWith(String(F(".gz"))))
        {
            return String(F("application/x-gzip"));
        }
        return String(F("text/plain"));
    }

    bool handleFileRead(String path)
    {
        if (path.endsWith("/"))
        {
            path += String(F("index.htm"));
        }
        String contentType = getContentType(path);
        String pathWithGz = path + String(F(".gz"));

        if (FileSystem::FileExists(pathWithGz) || FileSystem::FileExists(path))
        {
            if (FileSystem::FileExists(pathWithGz))
            {
                path += F(".gz");
            }
            File file = FileSystem::OpenFile(path, "r");
            _HTTP->streamFile(file, contentType);
            file.close();
            return true;
        }
        return false;
    }

    void handleFileUpload()
    {
        if (_HTTP->uri() != String(F("/edit")))
        {
            return;
        }

        HTTPUpload &upload = _HTTP->upload();

        if (upload.status == UPLOAD_FILE_START)
        {
            String filename = upload.filename;
            if (!filename.startsWith("/"))
            {
                filename = "/" + filename;
            }
            fsUploadFile = FileSystem::OpenFile(filename, "w");
        }
        else if (upload.status == UPLOAD_FILE_WRITE)
        {

            if (fsUploadFile)
            {
                fsUploadFile.write(upload.buf, upload.currentSize);
            }
        }
        else if (upload.status == UPLOAD_FILE_END)
        {

            if (fsUploadFile)
            {
                fsUploadFile.close();
            }
        }
    }

    void handleFileDelete()
    {

        if (_HTTP->args() == 0)
        {
            return _HTTP->send(500, String(F("text/plain")), String(F("BAD ARGS")));
        }
        String path = _HTTP->arg(0);

        if (path == "/")
        {
            return _HTTP->send(500, String(F("text/plain")), String(F("BAD PATH")));
        }
        if (!FileSystem::FileExists(path))
        {
            return _HTTP->send(404, String(F("text/plain")), String(F("FileNotFound")));
        }

        if (isEditableFile(path))
        {
            FileSystem::Remove(path);
        }

        _HTTP->send(200, String(F("text/plain")), "");
    }

    void handleFileCreate()
    {
        if (_HTTP->args() == 0)
        {
            return _HTTP->send(500, String(F("text/plain")), String(F("BAD ARGS")));
        }
        String path = _HTTP->arg(0);

        if (path == "/")
        {
            return _HTTP->send(500, String(F("text/plain")), String(F("BAD PATH")));
        }
        if (FileSystem::FileExists(path))
        {
            return _HTTP->send(500, String(F("text/plain")), String(F("FILE EXISTS")));
        }
        File file = FileSystem::OpenFile(path, "w");
        if (file)
        {
            file.close();
        }
        else
        {
            return _HTTP->send(500, String(F("text/plain")), String(F("CREATE FAILED")));
        }
        _HTTP->send(200, String(F("text/plain")), "");
    }

    void handleFileList()
    {
        if (!_HTTP->hasArg(String(F("dir"))))
        {
            _HTTP->send(500, String(F("text/plain")), String(F("BAD ARGS")));
            return;
        }

        String path = _HTTP->arg(String(F("dir")));
        Dir dir = FileSystem::OpenDir(path);

        String output = "[";
        while (dir.next())
        {
            File entry = dir.openFile("r");
            String entryName = String(entry.name());

            if (isEditableFile(entryName))
            {
                if (output != "[")
                {
                    output += ',';
                }
                bool isDir = false;
                output += String(F("{\"type\":\""));
                output += (isDir) ? String(F("dir")) : String(F("file"));
                output += String(F("\",\"name\":\""));
                output += entryName;
                output += String(F("\"}"));
            }

            entry.close();
        }

        output += "]";
        _HTTP->send(200, String(F("text/json")), output);
    }

    bool isEditableFile(String path)
    {
        if (path.endsWith(String(F(".gz"))) || path.endsWith(String(F("wifiConfig.txt"))))
        {
            return false;
        }
        return true;
    }

}