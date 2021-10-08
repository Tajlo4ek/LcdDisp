#pragma once

#include <Arduino.h>

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
    public:
        typedef std::function<void()> Callback;

        Clock(int utc);
        void SetTime(int hour, int minute, int second);
        void SetDate(int day, int month, int year);
        void Tick();

        const Time GetTime() const;
        const Date GetDate() const;
        const String GetDateString() const;

        void ParseFromNtp(unsigned long time);

        void SetTimeChangeCallback(Callback callback);
        void SetDateChangeCallback(Callback callback);

    private:
        Time nowTime;
        Date nowDate;

        long lastTickTime;
        int utc;

        Callback timeChanged;
        Callback dateChanged;

        static const byte monthDays[];
    };
}