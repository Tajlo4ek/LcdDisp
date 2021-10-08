#include "MainScreen.h"

#include "FileNames.h"
#include "Commands.h"
#include "Utils/WeatherImages.h"

#include "Utils/Internet/NtpTime.h"
#include "Utils/Internet/WifiUtils.h"
#include "Utils/FileSystem/FileSystem.h"
#include "Utils/Parsers/JsonParser.h"

#include "Utils/Logger/Logger.h"

namespace Screens
{
#define WEATHER_CONFIG_CITY F("city")
#define WEATHER_CONFIG_APIKEY F("apiKey")

    MainScreen::MainScreen(TFT_eSPI *lcd)
        : Screen(lcd)
    {
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
            this->GetWeather();
        };
        this->weatherTimer.SetInterval(10 * 60 * 1000);

        clockTimersManager.AddTimer(clockTimer);
        clockTimersManager.AddTimer(timeSyncTimer);
        clockTimersManager.AddTimer(resetSyncTimer);
        clockTimersManager.AddTimer(weatherTimer);
        clockTimersManager.StopAll();

        Controls::ControlRect controlRect = {2, 2, 156, 8};
        this->labelMessage = new Controls::Label(lcd, controlRect, Controls::Label::TextSize::Small);
        this->labelMessage->SetColor(DrawUtils::Get565Color(0, 0, 255), DrawUtils::Get565Color(0, 0, 0));

        controlRect = {2, 75, 156, 16};
        this->labelDate = new Controls::Label(lcd, controlRect, Controls::Label::TextSize::Big);
        this->labelDate->SetColor(DrawUtils::Get565Color(0, 0, 255), DrawUtils::Get565Color(0, 0, 0));

        this->digitalClock = new Controls::DigitalClock(lcd, 17);
        this->digitalClock->SetColor(DrawUtils::Get565Color(0, 0, 255), DrawUtils::Get565Color(0, 0, 0));

        controlRect = {64, 55, 32, 8};
        this->labelTimeSync = new Controls::Label(lcd, controlRect, Controls::Label::TextSize::Small);
        this->labelTimeSync->SetColor(DrawUtils::Get565Color(255, 0, 0), DrawUtils::Get565Color(0, 0, 0));
        this->labelTimeSync->SetVisible(false);
        this->labelTimeSync->DrawText(F("sync"), Controls::Label::TextAlignment::Center);

        controlRect = {0, 92, 32, 32};
        this->imageWeather = new Controls::Image(lcd, controlRect);
        this->imageWeather->SetColor(DrawUtils::Get565Color(0, 0, 255), DrawUtils::Get565Color(0, 0, 0));

        controlRect = {112, 100, 48, 16};
        this->labelTemp = new Controls::Label(lcd, controlRect, Controls::Label::TextSize::Big);
        this->labelTemp->SetColor(DrawUtils::Get565Color(0, 0, 255), DrawUtils::Get565Color(0, 0, 0));

        controlRect = {32, 92, 80, 32};
        this->labelWeatherDescription = new Controls::MultilineLable(lcd, controlRect, Controls::Label::TextSize::Small);
        this->labelWeatherDescription->SetColor(DrawUtils::Get565Color(0, 0, 255), DrawUtils::Get565Color(0, 0, 0));

        this->myClock.SetTimeChangeCallback(std::bind(&MainScreen::DrawTime, this));
        this->myClock.SetDateChangeCallback(std::bind(&MainScreen::DrawDate, this));

        String json = FileSystem::ReadFile(WEATHER_CONFIG_PATH);
        this->weatherCity = JsonParser::GetJsonData(json, WEATHER_CONFIG_CITY);
        this->weatherApiKey = JsonParser::GetJsonData(json, WEATHER_CONFIG_APIKEY);

        nowWeather = {99, F("weather not sync"), F("abort")};
    }

    void MainScreen::ReloadConfig()
    {
        //clockDrawer->ReloadConfig();
    }

    void MainScreen::EnterFocus()
    {
        lcd->fillScreen(DrawUtils::Get565Color(0, 0, 0));

        String message = F("IP: ");
        message += WifiUtils::GetIpString();
        this->labelMessage->DrawText(message, Controls::Label::TextAlignment::Center);

        this->isTimeSync = false;
        DrawTime();
        DrawDate();
        DrawWeather();

        this->CheckTimeSync();
        this->GetWeather();

        clockTimersManager.StartAll();
    }

    void MainScreen::DrawTime()
    {
        auto time = this->myClock.GetTime();

        this->digitalClock->DrawTime((byte)time.hour, (byte)time.minute, (int)(time.second) % 2 == 1);

        this->labelTimeSync->SetVisible(!isTimeSync);
    }

    void MainScreen::DrawDate()
    {
        this->labelDate->DrawText(myClock.GetDateString(), Controls::Label::TextAlignment::Center);
    }

    void MainScreen::DrawWeather()
    {
        this->imageWeather->DrawImage(
            WeatherImages::GetImage(nowWeather.imageName),
            WeatherImages::ImageByteCount);

        char tempStr[] = {'+', '_', '_', 'C', '\0'};

        int tempAbs = nowWeather.temp >= 0 ? nowWeather.temp : -nowWeather.temp;
        if (tempAbs < 10)
        {
            tempStr[0] = ' ';
            tempStr[1] = nowWeather.temp > 0 ? '+' : '-';
        }
        else
        {
            tempStr[0] = nowWeather.temp > 0 ? '+' : '-';
            tempStr[1] = tempAbs / 10 + '0';
        }

        tempStr[2] = tempAbs % 10 + '0';

        this->labelTemp->DrawText(String(tempStr), Controls::Label::TextAlignment::Right);
        this->labelWeatherDescription->DrawText(this->nowWeather.description, Controls::Label::TextAlignment::Left);
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
        if (hasEthernet == true)
        {
            if (isTimeSync == false)
            {
                auto time = NtpTime::Ask_NTP_Time(isTimeSync);
                if (isTimeSync)
                {
                    myClock.ParseFromNtp(time);
                }
            }
        }
        else
        {
            isTimeSync = false;
        }

        DrawTime();
    }

    void MainScreen::GetWeather()
    {
        if (hasEthernet == true)
        {
            bool isOk = false;
            auto weather = Weather::GetWether(isOk, this->weatherCity, this->weatherApiKey);

            nowWeather.imageName = weather.imageName;
            nowWeather.description = weather.description;
            if (isOk == true)
            {
                nowWeather.temp = weather.temp;
            }
        }

        DrawWeather();
    }

    MainScreen::~MainScreen()
    {
        delete labelMessage;
        delete labelDate;
        delete digitalClock;
        delete labelTimeSync;
        delete labelTemp;
        delete labelWeatherDescription;
    }

}