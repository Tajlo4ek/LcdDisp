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

        if (client.connect(host, 80) == 0)
        {
            ParseWeather(F("{\"cod\":404}"), weatherData, isOk);
            isOk = false;
            return weatherData;
        }

        client.print(F("GET "));
        client.print(F("/data/2.5/weather?q="));
        client.print(city);
        client.print(F("&appid="));
        client.print(apiKey);
        client.println(F(" HTTP/1.1"));

        client.print(F("Host: "));
        client.println(host);
        client.println(F("Connection: close"));
        client.println();

        //TODO: mb remove delay
        delay(1000);

        String json;
        while (client.available())
        {
            char next = (char)client.read();
            json += next;
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
            weather.description = F("not sync. error: ");
            weather.description += cod;
            isOk = false;

            String log = F("weather error. code:");
            log += cod;
            Logger::Log(log);
            return;
        }

        weather.description = JsonParser::GetJsonData(json, F("description"));
        weather.imageName = JsonParser::GetJsonData(json, F("icon"));
        String tempString = JsonParser::GetJsonData(json, F("temp"));

        auto firstChar = weather.description[0];
        //TODO:
        if (firstChar >= 'a' && firstChar <= 'z')
        {
            weather.description.setCharAt(0, firstChar - 32);
        }

        auto temp = tempString.toInt() - 273;

        if (temp <= 0)
        {
            weather.temp += '-';
        }
        else
        {
            weather.temp += '+';
        }
        weather.temp += temp;
        weather.temp += 'C';
        isOk = true;
    }

}