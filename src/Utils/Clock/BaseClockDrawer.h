#pragma once

#include <TFT_eSPI.h>
#include <Arduino.h>
#include "Clock.h"
#include "Utils/TrackedVal.h"
#include "Utils/ScreenDrawer.h"

namespace ClockDrawer
{
    class BaseClockDrawer : public ScreenDrawer::ScreenDrawer
    {
    protected:
        Clock::Clock *myClock;

        struct WeatherData
        {
            String temp;
            String description;
            String imageName;
        };

        TrackedVal::TrackedValue<String> *message;
        TrackedVal::TrackedValue<WeatherData> *weatherData;
        TrackedVal::TrackedValue<bool> *isTimeSync;

        bool isWeatherOk;

    private:
        virtual void WeatherChanged() = 0;
        virtual void MessageChanged() = 0;
        virtual void TimeChanged() = 0;
        virtual void DateChanged() = 0;
        virtual void TimeSyncChanged() = 0;

    public:
        BaseClockDrawer(TFT_eSPI &lcd, int width, int height, Clock::Clock &clock);

        void SetMessage(String message);
        void SetWeather(int temp, String type, String imageName);
        void SetTimeSync(bool isSync);

        ~BaseClockDrawer();
    };

}