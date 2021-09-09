#include "Weather.h"

#include "Utils/Parsers/JsonParser.h"

namespace Weather
{
    void ParseWeather(String &json, WeatherData &weather, bool &isOk);
    WeatherData GetWether(NotBlockDelay notBlockDelay, bool &isOk, String city, String apiKey);

    WeatherData GetWether(NotBlockDelay notBlockDelay, bool &isOk, String city, String apiKey)
    {
        auto host = F("api.openweathermap.org");

        WeatherData weatherData;
        WiFiClient client;
        const int httpPort = 80;

        if (!client.connect(host, httpPort))
        {
            isOk = false;
            return weatherData;
        }

        client.print(String(F("GET ")));
        client.print("/data/2.5/weather?q=");
        client.print(city);
        client.print("&appid=");
        client.print(apiKey);
        client.println(String(F(" HTTP/1.1")));

        client.print(String(F("Host: ")));
        client.println(host);
        client.println(F("Connection: close"));
        client.println();

        //TODO: mb client.flush()
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

        ParseWeather(json, weatherData, isOk);

        return weatherData;
    }

    void ParseWeather(String &json, WeatherData &weather, bool &isOk)
    {
        auto description = JsonParser::GetJsonData(json, F("description"), isOk);
        if (!isOk)
        {
            return;
        }

        auto icon = JsonParser::GetJsonData(json, F("icon"), isOk);
        if (!isOk)
        {
            return;
        }

        auto temp = JsonParser::GetJsonData(json, F("temp"), isOk);
        if (!isOk)
        {
            return;
        }

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

}