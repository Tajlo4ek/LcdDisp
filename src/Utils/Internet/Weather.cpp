#include "Weather.h"

namespace Weather
{
    void parseJson(String &json, WeatherData &weather, bool &isOk);
    WeatherData GetWether(NonBlockDelay nonBlockDelay, bool &isOk);
    String GetJsonData(String &json, String name, bool &isOk);

    WeatherData GetWether(NonBlockDelay nonBlockDelay, bool &isOk)
    {
        WeatherData weatherData;
        WiFiClient client;
        const int httpPort = 80;
        if (!client.connect(HOST, httpPort))
        {
            isOk = false;
            return weatherData;
        }

        client.println(String(F("GET ")) + PATH + String(F(" HTTP/1.1")));
        client.println(String(F("Host: ")) + HOST);
        client.println(F("Connection: close"));
        client.println();

        delay(1000);

        String json;
        int count = 0;
        while (client.available())
        {
            char next = (char)client.read();
            if (next == '{')
            {
                count++;
            }
            else if (next == '}')
            {
                count--;
            }
            if (count)
            {
                json += next;
            }
        }
        json += '}';

        parseJson(json, weatherData, isOk);

        return weatherData;
    }

    void parseJson(String &json, WeatherData &weather, bool &isOk)
    {
        auto description = GetJsonData(json, F("\"description\""), isOk);
        if (!isOk)
        {
            return;
        }

        auto icon = GetJsonData(json, F("\"icon\""), isOk);
        if (!isOk)
        {
            return;
        }

        auto temp = GetJsonData(json, F("\"temp\""), isOk);
        if (!isOk)
        {
            return;
        }

        description.replace("\"", "");
        icon.replace("\"", "");
        temp.replace("\"", "");

        auto firstChar = description[0];
        if (firstChar >= 'a' && firstChar <= 'z')
        {
            description.setCharAt(0, firstChar - 32);
        }

        weather.temp = temp.toInt() - 273;
        weather.imageName = icon;
        weather.description = description;

        isOk = true;
    }

    String GetJsonData(String &json, String name, bool &isOk)
    {
        auto dataStart = json.indexOf(name);
        if (dataStart == -1)
        {
            isOk = false;
            return "";
        }

        int startInd = json.indexOf(':', dataStart);
        if (startInd == -1)
        {
            isOk = false;
            return "";
        }

        String res = "";
        int openBracket = 0;
        for (uint i = startInd + 1; i < json.length(); i++)
        {
            auto ch = json[i];
            if (ch == '[' || ch == '{')
            {
                openBracket++;
            }
            else if (ch == ']' || ch == '}')
            {
                openBracket--;
            }

            if ((ch == ',' && openBracket == 0) || openBracket < 0)
            {
                break;
            }

            res += ch;
        }

        isOk = true;
        return res;
    }

}