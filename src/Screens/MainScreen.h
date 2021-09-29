#pragma once

#include "BaseScreen.h"

#include "Utils/Timer/MillisTimer.h"
#include "Utils/Timer/TimerManager.h"
#include "Utils/Clock/Clock.h"
#include "Utils/Clock/DigitalClockDrawer.h"

namespace MainScreen
{
    class MainScreen : public BaseScreen::Screen
    {
        typedef std::function<void(unsigned int)> NotBlockDelay;

    private:
        void GetWeather();
        void CheckTimeSync();

        bool isTimeSync;

        Clock::Clock myClock;
        ClockDrawer::BaseClockDrawer *clockDrawer;
        MillisTimer::Timer clockTimer;
        MillisTimer::Timer timeSyncTimer;
        MillisTimer::Timer resetSyncTimer;
        MillisTimer::Timer weatherTimer;
        TimerManager::TimerManager clockTimersManager;

        NotBlockDelay notBlockDelay;

        String weatherCity;
        String weatherApiKey;

    public:
        void EnterFocus() override;
        void LeaveFocus() override;
        void Loop() override;
        String ParseMessage(const String &message) override;
        void ReloadConfig() override;

        MainScreen(TFT_eSPI &lcd, int lcdWidth, int lcdHeight, BaseScreen::OnScreenWorkEnd onWorkEnd, NotBlockDelay notBlockDelay);
        ~MainScreen();
    };
}