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

#define CONFIG_BACK_COLOR F("backColor")
#define CONFIG_CLOCK_MAIN_COLOR F("clockMainColor")
#define CONFIG_CLOCK_SECOND_COLOR F("clockSecondColor")
#define CONFIG_ERROR_COLOR F("errorColor")

    MainScreen::MainScreen(TFT_eSPI *lcd)
        : Screen(lcd)
    {
        this->isTimeSync = false;

        //clock tick 500 ms
        this->clockTimer = MillisTimer::Timer([this]
                                              { this->myClock.Tick(); },
                                              500);

        this->timeSyncTimer = MillisTimer::Timer(std::bind(&MainScreen::CheckTimeSync, this), 100);
        this->weatherTimer = MillisTimer::Timer(std::bind(&MainScreen::GetWeather, this), 100);

        timerManager.AddTimer(clockTimer);
        timerManager.AddTimer(timeSyncTimer);
        timerManager.AddTimer(weatherTimer);
        timerManager.StopAll();

        Controls::ControlRect controlRect = {0, 2, 160, 8};
        this->labelMessage = new Controls::Label(lcd, controlRect, Controls::Label::TextSize::Small);
        String message = F("IP: ");
        message += WifiUtils::GetIpString();
        this->labelMessage->DrawText(message, Controls::Label::TextAlignment::Center);
        controls.push_back(this->labelMessage);

        controlRect = {0, 12, 160, 50};
        this->digitalClock = new Controls::DigitalClock(lcd, controlRect);
        controls.push_back(this->digitalClock);

        controlRect = {0, 66, 160, 16};
        this->labelDate = new Controls::Label(lcd, controlRect, Controls::Label::TextSize::Big);
        controls.push_back(this->labelDate);

        controlRect = {0, 83, 160, 8};
        this->labelTimeSync = new Controls::Label(lcd, controlRect, Controls::Label::TextSize::Small);
        this->labelTimeSync->SetVisible(false);
        this->labelTimeSync->DrawText(F("time not sync"), Controls::Label::TextAlignment::Center);
        controls.push_back(this->labelTimeSync);

        controlRect = {0, 92, 32, 32};
        this->imageWeather = new Controls::Image(lcd, controlRect);
        controls.push_back(this->imageWeather);

        controlRect = {112, 100, 48, 16};
        this->labelTemp = new Controls::Label(lcd, controlRect, Controls::Label::TextSize::Big);
        controls.push_back(this->labelTemp);

        controlRect = {33, 100, 79, 24};
        this->labelWeatherDescription = new Controls::MultilineLable(lcd, controlRect, Controls::Label::TextSize::Small);
        controls.push_back(this->labelWeatherDescription);

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

        const uint colorCount = 4;
        String colorNames[colorCount]{
            CONFIG_BACK_COLOR,
            CONFIG_CLOCK_MAIN_COLOR,
            CONFIG_CLOCK_SECOND_COLOR,
            CONFIG_ERROR_COLOR,
        };

        uint16_t clockMainColor;
        uint16_t clockSecondColor;
        uint16_t errorColor;

        uint16_t *colors[colorCount]{
            &this->backColor,
            &clockMainColor,
            &clockSecondColor,
            &errorColor};

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
            this->labelTimeSync->SetMainColor(errorColor);

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
        const uint configCount = 4;
        String configNames[configCount]{
            CONFIG_BACK_COLOR,
            CONFIG_CLOCK_MAIN_COLOR,
            CONFIG_CLOCK_SECOND_COLOR,
            CONFIG_ERROR_COLOR};

        String datas[configCount]{
            DrawUtils::GetJsonColor(0, 0, 0),
            DrawUtils::GetJsonColor(0, 0, 255),
            DrawUtils::GetJsonColor(0, 0, 200),
            DrawUtils::GetJsonColor(255, 0, 0),
        };

        FileSystem::WriteFile(
            MAIN_SCREEN_CONFIG_PATH,
            JsonParser::BuildJson(configNames, datas, configCount));
    }

    void MainScreen::ReDraw()
    {
        ClearScreen();
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
        ReDraw();
        timerManager.ResumeAll();
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
        timerManager.StopAll();
    }

    void MainScreen::Loop()
    {
        timerManager.Tick();
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
                // if notsync then next 1 minute else 1h
                this->timeSyncTimer.SetInterval(isTimeSync == false ? 60 * 1000 : 60 * 60 * 1000);
            }
        }
        else
        {
            isTimeSync = false;
            this->timeSyncTimer.SetInterval(0);
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

            // if notsync then next 1m else 10m
            this->weatherTimer.SetInterval(isOk == false ? 60 * 1000 : 10 * 60 * 1000);
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