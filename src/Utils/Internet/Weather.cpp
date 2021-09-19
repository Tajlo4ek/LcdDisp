#include "Weather.h"

#include "Utils/Parsers/JsonParser.h"
#include "Utils/Logger/Logger.h"

namespace Weather
{
    void ParseWeather(const String &json, WeatherData &weather, bool &isOk);
    WeatherData GetWether(NotBlockDelay notBlockDelay, bool &isOk, const String &city, const String &apiKey);

    WeatherData GetWether(NotBlockDelay notBlockDelay, bool &isOk, const String &city, const String &apiKey)
    {
        auto host = F("api.openweathermap.org");

        WeatherData weatherData;
        WiFiClient client;
        const int httpPort = 80;

        if (client.connect(host, httpPort) == 0)
        {
            isOk = false;
            return weatherData;
        }

        client.print(String(F("GET ")));
        client.print(F("/data/2.5/weather?q="));
        client.print(city);
        client.print(F("&appid="));
        client.print(apiKey);
        client.println(String(F(" HTTP/1.1")));

        client.print(String(F("Host: ")));
        client.println(host);
        client.println(F("Connection: close"));
        client.println();

        //TODO: mb client.flush()
        delay(1000);

        String json = String();
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

        ParseWeather(json, weatherData, isOk);

        return weatherData;
    }

    void ParseWeather(const String &json, WeatherData &weather, bool &isOk)
    {
        auto cod = JsonParser::GetJsonData(json, F("cod")).toInt();
        if (cod != 200)
        {
            weather.imageName = F("abort");
            weather.description = String(F("not sync. error: ")) + String(cod);
            isOk = false;
            Logger::Log(json);
            return;
        }

        auto description = JsonParser::GetJsonData(json, F("description"));
        auto icon = JsonParser::GetJsonData(json, F("icon"));
        auto tempString = JsonParser::GetJsonData(json, F("temp"));

        auto firstChar = description[0];
        if (firstChar >= 'a' && firstChar <= 'z')
        {
            description.setCharAt(0, firstChar - 32);
        }

        auto temp = tempString.toInt() - 273;
        auto bufTemp = String(temp) + 'C';

        if (temp == 0)
        {
            bufTemp = '-' + bufTemp;
        }
        else if (temp > 0)
        {
            bufTemp = '+' + bufTemp;
        }

        weather.temp = bufTemp;
        weather.imageName = icon;
        weather.description = description;
        isOk = true;
    }

}