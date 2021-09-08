#pragma once

#include <Arduino.h>
#include <WiFiClient.h>

namespace Weather
{
    struct WeatherData
    {
        int temp;
        String description;
        String imageName;
    };

#define HOST "api.openweathermap.org"
#define PATH "/data/2.5/weather?q=Voronezh&appid=69c984a11b9b311eea8fc078963c59ff"

    typedef void (*NonBlockDelay)(unsigned long);

    WeatherData GetWether(NonBlockDelay nonBlockDelay, bool &isOk);

}