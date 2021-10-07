#include "Weather.h"

#include "Utils/Parsers/JsonParser.h"
#include "Utils/Logger/Logger.h"

namespace Weather
{
    void ParseWeather(const String &json, WeatherData &weather, bool &isOk);
    WeatherData GetWether(bool &isOk, const String &city, const String &apiKey);

    WeatherData GetWether(bool &isOk, const String &city, const String &apiKey)
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

        //TODO: code 0 ?
        auto cod = JsonParser::GetJsonData(json, F("cod")).toInt();
        if (cod != 200)
        {
            weather.imageName = F("abort");
            weather.description = F("not sync. error: ");
            weather.description += cod;
            isOk = false;

            String log = F("weather error. code:");
            log += cod;
            log += F("json: ");
            log += json;
            Logger::Log(log);
            return;
        }

        //TODO: check ok
        weather.description = JsonParser::GetJsonData(json, F("description"));
        weather.imageName = JsonParser::GetJsonData(json, F("icon"));
        weather.temp = JsonParser::GetJsonData(json, F("temp")).toInt() - 273;

        auto firstChar = weather.description[0];
        //first char to upper
        if (firstChar >= 'a' && firstChar <= 'z')
        {
            weather.description.setCharAt(0, firstChar - ('a' - 'A'));
        }
        isOk = true;
    }

}