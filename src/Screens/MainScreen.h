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

namespace MainScreen
{

    //TODO: make not fixed size
    class MainScreen : public BaseScreen::Screen
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
        TimerManager::TimerManager clockTimersManager;

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

    public:
        void EnterFocus() override;
        void LeaveFocus() override;
        void Loop() override;
        void ReloadConfig() override;

        MainScreen(TFT_eSPI *lcd, BaseScreen::OnScreenWorkEnd onWorkEnd);
        ~MainScreen();
    };
}