#pragma once

#include <Arduino.h>
#include <WiFiClient.h>

namespace Weather
{
    struct WeatherData
    {
        String temp;
        String description;
        String imageName;
    };

    WeatherData GetWether(bool &isOk, const String &city, const String &apiKey);
}