#pragma once

#include <TFT_eSPI.h>
#include <Arduino.h>
#include "Utils/Clock/Clock.h"
#include "Utils/TrackedVal.h"
#include "Utils/Drawers/ScreenDrawer.h"
#include "Utils/Internet/Weather.h"

namespace Drawers
{
    class BaseClockDrawer : public ScreenDrawer::ScreenDrawer
    {
    protected:
        Clock::Clock *myClock;

        TrackedVal::TrackedValue<String> *message;
        TrackedVal::TrackedValue<Weather::WeatherData> *weatherData;
        TrackedVal::TrackedValue<bool> *isTimeSync;

    private:
        virtual void WeatherChanged() = 0;
        virtual void MessageChanged() = 0;
        virtual void TimeChanged() = 0;
        virtual void DateChanged() = 0;
        virtual void TimeSyncChanged() = 0;

    public:
        BaseClockDrawer(TFT_eSPI &lcd, int width, int height, Clock::Clock &clock);
        virtual void Init() = 0;

        void SetMessage(const String &message);
        void SetWeather(Weather::WeatherData weatherData, bool isError);
        void SetTimeSync(bool isSync);

        ~BaseClockDrawer();
    };

}