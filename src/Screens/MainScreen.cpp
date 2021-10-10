#include "MainScreen.h"

#include "FileNames.h"
#include "Commands.h"
#include "Utils/WeatherImages.h"

#include "Utils/Internet/NtpTime.h"
#include "Utils/Internet/WifiUtils.h"
#include "Utils/FileSystem/FileSystem.h"
#include "Utils/Parsers/JsonParser.h"

#include "Utils/DrawUtils/Color.h"

namespace Screens
{
#define WEATHER_CONFIG_CITY F("city")
#define WEATHER_CONFIG_APIKEY F("apiKey")

#define CONFIG_BACK_COLOR String(F("backColor"))
#define CONFIG_CLOCK_MAIN_COLOR String(F("clockMainColor"))
#define CONFIG_CLOCK_SECOND_COLOR String(F("clockSecondColor"))

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

        controlRect = {2, 75, 156, 16};
        this->labelDate = new Controls::Label(lcd, controlRect, Controls::Label::TextSize::Big);

        controlRect = {0, 17, 160, 58};
        this->digitalClock = new Controls::DigitalClock(lcd, controlRect);

        controlRect = {64, 55, 32, 8};
        this->labelTimeSync = new Controls::Label(lcd, controlRect, Controls::Label::TextSize::Small);
        this->labelTimeSync->SetVisible(false);
        this->labelTimeSync->DrawText(F("sync"), Controls::Label::TextAlignment::Center);

        controlRect = {0, 92, 32, 32};
        this->imageWeather = new Controls::Image(lcd, controlRect);

        controlRect = {112, 100, 48, 16};
        this->labelTemp = new Controls::Label(lcd, controlRect, Controls::Label::TextSize::Big);

        controlRect = {33, 92, 79, 32};
        this->labelWeatherDescription = new Controls::MultilineLable(lcd, controlRect, Controls::Label::TextSize::Small);

        this->myClock.SetTimeChangeCallback(std::bind(&MainScreen::DrawTime, this));
        this->myClock.SetDateChangeCallback(std::bind(&MainScreen::DrawDate, this));

        nowWeather = {99, F("weather not sync"), F("abort")};

        ReloadConfig();
    }

    void MainScreen::ReloadConfig()
    {
        String json = FileSystem::ReadFile(WEATHER_CONFIG_PATH);
        this->weatherCity = JsonParser::GetJsonData(json, WEATHER_CONFIG_CITY);
        this->weatherApiKey = JsonParser::GetJsonData(json, WEATHER_CONFIG_APIKEY);

        json = FileSystem::ReadFile(MAIN_SCREEN_CONFIG_PATH);
        if (json.isEmpty())
        {
            this->CreateDefaultConfig();
            json = FileSystem::ReadFile(MAIN_SCREEN_CONFIG_PATH);
        }

        const uint colorCount = 3;
        String colorNames[colorCount]{
            CONFIG_BACK_COLOR,
            CONFIG_CLOCK_MAIN_COLOR,
            CONFIG_CLOCK_SECOND_COLOR,
        };

        uint16_t clockMainColor;
        uint16_t clockSecondColor;

        uint16_t *colors[colorCount]{
            &this->backColor,
            &clockMainColor,
            &clockSecondColor,
        };

        bool loadRes = DrawUtils::LoadColorsFromJson(json, colorNames, colors, colorCount);

        if (loadRes == false)
        {
            this->CreateDefaultConfig();
            this->ReloadConfig();
        }
        else
        {
            this->labelMessage->SetBackColor(this->backColor);
            this->labelMessage->SetMainColor(clockMainColor);

            this->labelDate->SetBackColor(this->backColor);
            this->labelDate->SetMainColor(clockMainColor);

            this->labelTimeSync->SetBackColor(this->backColor);
            this->labelTimeSync->SetMainColor(clockMainColor);

            this->imageWeather->SetBackColor(this->backColor);
            this->imageWeather->SetMainColor(clockMainColor);

            this->labelTemp->SetBackColor(this->backColor);
            this->labelTemp->SetMainColor(clockMainColor);

            this->labelWeatherDescription->SetBackColor(this->backColor);
            this->labelWeatherDescription->SetMainColor(clockMainColor);

            this->digitalClock->SetBackColor(this->backColor);
            this->digitalClock->SetMainColor(clockMainColor);
            this->digitalClock->SetClockSecondColor(clockSecondColor);
        }
    }

    void MainScreen::CreateDefaultConfig()
    {
        const uint configCount = 3;
        String configNames[configCount]{
            CONFIG_BACK_COLOR,
            CONFIG_CLOCK_MAIN_COLOR,
            CONFIG_CLOCK_SECOND_COLOR,
        };

        String datas[configCount]{
            DrawUtils::GetJsonColor(0, 0, 0),
            DrawUtils::GetJsonColor(0, 0, 255),
            DrawUtils::GetJsonColor(0, 0, 200),
        };

        FileSystem::WriteFile(
            MAIN_SCREEN_CONFIG_PATH,
            JsonParser::BuildJson(configNames, datas, configCount));
    }

    void MainScreen::ReDraw()
    {
        lcd->fillScreen(this->backColor);
        this->labelMessage->ReDraw();
        this->labelDate->ReDraw();
        this->labelTimeSync->ReDraw();
        this->imageWeather->ReDraw();
        this->labelTemp->ReDraw();
        this->labelWeatherDescription->ReDraw();
        this->digitalClock->ReDraw();
    }

    void MainScreen::EnterFocus()
    {
        lcd->fillScreen(this->backColor);

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