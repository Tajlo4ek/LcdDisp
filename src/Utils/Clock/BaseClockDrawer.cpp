#include "BaseClockDrawer.h"

namespace ClockDrawer
{
    BaseClockDrawer::BaseClockDrawer(TFT_eSPI &lcd, int width, int height, Clock::Clock &clock) : ScreenDrawer(lcd, width, height)
    {
        this->myClock = &clock;

        this->message = new TrackedVal::TrackedValue<String>(String(), std::bind(&BaseClockDrawer::MessageChanged, this));
        this->weatherData = new TrackedVal::TrackedValue<Weather::WeatherData>(Weather::WeatherData(), std::bind(&BaseClockDrawer::WeatherChanged, this));
        this->isTimeSync = new TrackedVal::TrackedValue<bool>(false, std::bind(&BaseClockDrawer::TimeSyncChanged, this));

        this->myClock->SetTimeChangeCallback(std::bind(&BaseClockDrawer::TimeChanged, this));
        this->myClock->SetDateChangeCallback(std::bind(&BaseClockDrawer::DateChanged, this));
    }

    void BaseClockDrawer::SetMessage(const String &message)
    {
        this->message->SetValue(message);
    }

    void BaseClockDrawer::SetWeather(Weather::WeatherData weatherData, bool isError)
    {
        Weather::WeatherData data;

        if (isError)
        {
            data.temp = this->weatherData->GetPrevievValue().temp;
            data.imageName = F("abort");
        }
        else
        {
            data.temp = weatherData.temp;
            data.imageName = weatherData.imageName;
        }

        data.description = weatherData.description;

        this->weatherData->SetValue(data);
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