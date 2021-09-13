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

    typedef std::function<void(unsigned int)> NotBlockDelay;

    WeatherData GetWether(NotBlockDelay notBlockDelay, bool &isOk, String city, String apiKey);
}