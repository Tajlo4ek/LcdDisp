
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
        String bData = LOG_TOKEN;
        bData += millis();
        bData += F(": ");
        bData += data;
        bData += COMMAND_STOP_CHAR;

        return bData;
    }

}
