#pragma once

#include <Arduino.h>
#include "Utils/TrackedVal.h"

namespace Clock
{
    struct Time
    {
        float hour;
        float minute;
        float second;
        float milliSecond;
    };

    struct Date
    {
        int day;
        int month;
        int year;
    };

    class Clock
    {
    private:
        TrackedVal::TrackedValue<Time> *time;
        TrackedVal::TrackedValue<Date> *date;

        long lastTickTime;

        const byte monthDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    public:
        Clock();
        void SetTime(int hour, int minute, int second);
        void SetDate(int day, int month, int year);
        void Tick();

        const Time GetTime() const;
        const Date GetDate() const;
        const String GetDateString() const;

        void ParseFromNtp(unsigned long time);

        void SetTimeChangeCallback(TrackedVal::Callback callback);
        void SetDateChangeCallback(TrackedVal::Callback callback);
    };
}