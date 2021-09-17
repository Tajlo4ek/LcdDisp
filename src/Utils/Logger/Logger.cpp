
#include "Logger.h"
#include "Utils/HTTP/HttpServer.h"
#include "Commands.h"

namespace Logger
{
    String BuildData(const String &data);

    bool isSerialInit = false;

    void SerialLog(const String &data)
    {
        Serial.print(BuildData(data));
    }

    void WebLog(const String &data)
    {
        HttpServer::AddWebLog(BuildData(data));
    }

    void Log(const String &data)
    {
        //SerialLog(data);
        WebLog(data);
    }

    String BuildData(const String &data)
    {
        return Commands::logToken + String(millis()) + String(F(": ")) + data + Commands::stopChar;
    }

}
