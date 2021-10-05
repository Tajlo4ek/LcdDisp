#include "MainScreen.h"

#include "FileNames.h"
#include "Commands.h"

#include "Utils/Internet/NtpTime.h"
#include "Utils/Internet/Weather.h"
#include "Utils/Internet/WifiUtils.h"
#include "Utils/FileSystem/FileSystem.h"
#include "Utils/Parsers/JsonParser.h"

namespace MainScreen
{
#define WEATHER_CONFIG_CITY F("city")
#define WEATHER_CONFIG_APIKEY F("apiKey")

    MainScreen::MainScreen(TFT_eSPI *lcd, int lcdWidth, int lcdHeight, BaseScreen::OnScreenWorkEnd onWorkEnd, NotBlockDelay notBlockDelay)
        : BaseScreen::Screen(lcd, onWorkEnd)
    {
        this->clockDrawer = new Drawers::DigitalClockDrawer(lcd, lcdWidth, lcdHeight, this->myClock);

        this->notBlockDelay = notBlockDelay;
        this->isTimeSync = false;

        //clock tick 500 ms
        this->clockTimer.callback = [this]
        {
            this->myClock.Tick();
        };
        this->clockTimer.SetInterval(500);

        //check is need sync time 1 minute
        this->timeSyncTimer.callback = std::bind(&MainScreen::CheckTimeSync, this);
        this->timeSyncTimer.SetInterval(60 * 1000);

        //reset sync time 1 hour
        this->resetSyncTimer.callback = [this]
        {
            this->isTimeSync = false;
        };
        this->resetSyncTimer.SetInterval(60 * 60 * 1000);

        //update weather 10 minute
        this->weatherTimer.callback = [this]
        {
            this->myClock.Tick();
        };
        this->weatherTimer.SetInterval(10 * 60 * 1000);

        clockTimersManager.AddTimer(clockTimer);
        clockTimersManager.AddTimer(timeSyncTimer);
        clockTimersManager.AddTimer(resetSyncTimer);
        clockTimersManager.AddTimer(weatherTimer);

        clockTimersManager.StopAll();

        String json = FileSystem::ReadFile(WEATHER_CONFIG_PATH);
        this->weatherCity = JsonParser::GetJsonData(json, WEATHER_CONFIG_CITY);
        this->weatherApiKey = JsonParser::GetJsonData(json, WEATHER_CONFIG_APIKEY);
    }

    String MainScreen::ParseMessage(const String &message)
    {
        return String();
    }

    void MainScreen::ReloadConfig()
    {
        clockDrawer->ReloadConfig();
    }

    void MainScreen::EnterFocus()
    {
        clockDrawer->Init();

        Weather::WeatherData defaultWeaterData;
        defaultWeaterData.temp = String('+');
        defaultWeaterData.temp += 666;
        defaultWeaterData.temp += 'C';
        defaultWeaterData.description = F("weather not sync");
        defaultWeaterData.imageName = F("abort");
        clockDrawer->SetWeather(defaultWeaterData, false);

        String message = F("IP: ");
        message += WifiUtils::GetIpString();
        clockDrawer->SetMessage(message);
        this->isTimeSync = false;
        this->CheckTimeSync();
        this->GetWeather();
        clockTimersManager.StartAll();
    }

    void MainScreen::LeaveFocus()
    {
        clockTimersManager.StopAll();
    }

    void MainScreen::Loop()
    {
        clockTimersManager.Tick();
    }

    void MainScreen::CheckTimeSync()
    {
        if (hasEthernet == false)
        {
            clockDrawer->SetTimeSync(false);
            return;
        }

        if (isTimeSync == false)
        {
            auto time = NtpTime::Ask_NTP_Time(this->notBlockDelay, isTimeSync);
            if (isTimeSync)
            {
                myClock.ParseFromNtp(time);
            }
            clockDrawer->SetTimeSync(isTimeSync);
        }
    }

    void MainScreen::GetWeather()
    {
        if (hasEthernet == false)
        {
            return;
        }

        bool isOk = false;
        auto weather = Weather::GetWether(this->notBlockDelay, isOk, this->weatherCity, this->weatherApiKey);
        clockDrawer->SetWeather(weather, !isOk);
    }

    MainScreen::~MainScreen()
    {
    }

}