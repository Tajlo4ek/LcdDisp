#include "BaseClockDrawer.h"

namespace ClockDrawer
{
    BaseClockDrawer::BaseClockDrawer(TFT_eSPI &lcd, int width, int height, Clock::Clock &clock) : ScreenDrawer(lcd, width, height)
    {
        this->myClock = &clock;

        this->message = new TrackedVal::TrackedValue<String>(String(""), std::bind(&BaseClockDrawer::MessageChanged, this));
        this->weatherData = new TrackedVal::TrackedValue<WeatherData>(WeatherData(), std::bind(&BaseClockDrawer::WeatherChanged, this));
        this->isTimeSync = new TrackedVal::TrackedValue<bool>(false, std::bind(&BaseClockDrawer::TimeSyncChanged, this));

        this->myClock->SetTimeChangeCallback(std::bind(&BaseClockDrawer::TimeChanged, this));
        this->myClock->SetDateChangeCallback(std::bind(&BaseClockDrawer::DateChanged, this));
    }

    void BaseClockDrawer::SetMessage(String message)
    {
        this->message->SetValue(message);
    }

    void BaseClockDrawer::SetWeather(int temp, String description, String imageName)
    {
        auto buf = String(temp) + "C";

        if (temp == 0)
        {
            buf = '-' + buf;
        }
        else if (temp > 0)
        {
            if (temp >= 1000)
            {
                buf = "";
            }
            else
            {
                buf = '+' + buf;
            }
        }

        WeatherData data;
        data.temp = buf;
        data.description = description;
        data.imageName = imageName;

        this->weatherData->SetValue(data);
        this->isWeatherOk = true;
    }

    void BaseClockDrawer::SetTimeSync(bool isSync)
    {
        this->isTimeSync->SetValue(isSync);
    }

    BaseClockDrawer::~BaseClockDrawer()
    {
        delete this->message;
        delete this->weatherData;
        this->lcd = nullptr;
        this->myClock = nullptr;
    }

}