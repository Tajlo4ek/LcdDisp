
#include "Logger.h"
#include "Utils/HTTP/HttpServer.h"
#include "Commands.h"

namespace Logger
{
    String BuildData(String &data);

    bool isSerialInit = false;

    void SerialLog(String data)
    {
        Serial.print(BuildData(data));
    }

    void WebLog(String data)
    {
        HttpServer::AddWebLog(BuildData(data));
    }

    String BuildData(String &data)
    {
        return Commands::logToken + String(millis()) + String(F(": ")) + data + Commands::stopChar;
    }

}
