#pragma once

#include "BaseScreen.h"

#include "Utils/Timer/MillisTimer.h"
#include "Utils/Timer/TimerManager.h"
#include "Utils/Clock/Clock.h"
#include "Utils/Internet/Weather.h"

#include "Controls/Label.h"
#include "Controls/DigitalClock.h"
#include "Controls/Image.h"
#include "Controls/MultilineLable.h"

namespace Screens
{
    class MainScreen : public Screen
    {

    private:
        void GetWeather();
        void CheckTimeSync();

        bool isTimeSync;

        Clock::Clock myClock = Clock::Clock(3);
        MillisTimer::Timer clockTimer;
        MillisTimer::Timer timeSyncTimer;
        MillisTimer::Timer resetSyncTimer;
        MillisTimer::Timer weatherTimer;
        TimerManager::TimerManager timerManager;

        String weatherCity;
        String weatherApiKey;
        Weather::WeatherData nowWeather;

        Controls::Label *labelMessage;

        Controls::Label *labelDate;
        Controls::Label *labelTimeSync;
        Controls::DigitalClock *digitalClock;

        Controls::Image *imageWeather;
        Controls::Label *labelTemp;
        Controls::MultilineLable *labelWeatherDescription;

        void DrawTime();
        void DrawDate();
        void DrawWeather();

        void CreateDefaultConfig() override;
        void EnterFocus() override;

    public:
        void LeaveFocus() override;
        void Loop() override;
        void ReloadConfig() override;
        void ReDraw() override;

        MainScreen(TFT_eSPI *lcd);
        ~MainScreen();
    };
}