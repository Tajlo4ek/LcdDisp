#include "Clock.h"

namespace Clock
{
    Clock::Clock()
    {
        Time time;
        Date date;

        this->time = new TrackedVal::TrackedValue<Time>(time);
        this->date = new TrackedVal::TrackedValue<Date>(date);

        this->SetTime(0, 0, 0);
        this->SetDate(1, 1, 1970);
    }

    void Clock::SetTime(int hour, int minute, int second)
    {
        this->lastTickTime = millis();

        Time nextTime;
        nextTime.milliSecond = 0;
        nextTime.second = second;
        nextTime.minute = (minute * 60 + second) / 60.0;
        nextTime.hour = (hour * 60 * 60 + minute * 60 + second) / 60.0 / 60.0;

        this->time->SetValue(nextTime);
    }

    void Clock::SetDate(int day, int month, int year)
    {
        Date nextDate;
        nextDate.day = day;
        nextDate.month = month;
        nextDate.year = year;
        this->date->SetValue(nextDate);
    }

    void Clock::Tick()
    {
        unsigned long delta = millis() - this->lastTickTime;
        this->lastTickTime = millis();

        Time nextTime = this->time->GetValue();
        nextTime.milliSecond += delta;
        nextTime.second += delta / 1000.0;
        nextTime.minute += delta / 1000.0 / 60.0;
        nextTime.hour += delta / 1000.0 / 60.0 / 60.0;

        while (nextTime.milliSecond >= 1000)
        {
            nextTime.milliSecond -= 1000;
        }

        while (nextTime.second >= 60)
        {
            nextTime.second -= 60;
        }

        while (nextTime.minute >= 60)
        {
            nextTime.minute -= 60;
        }

        if (nextTime.hour >= 24)
        {
            Date nextDate = this->date->GetValue();

            nextTime.hour -= 24;

            nextDate.day++;
            byte monthLength = 0;
            if (nextDate.month == 2)
            { // february
                monthLength = nextDate.year % 4 == 0 ? 29 : 28;
            }
            else
            {
                monthLength = this->monthDays[nextDate.month - 1];
            }
            if (nextDate.day > monthLength)
            {
                nextDate.day = 1;
                nextDate.month++;
                if (nextDate.month >= 13)
                {
                    nextDate.month = 1;
                    nextDate.year++;
                }
            }

            this->date->SetValue(nextDate);
        }

        this->time->SetValue(nextTime);
    }

    const Date Clock::GetDate() const
    {
        return this->date->GetValue();
    }

    const Time Clock::GetTime() const
    {
        return this->time->GetValue();
    }

    const String Clock::GetDateString() const
    {
        char res[] = "00.00.0000";

        Date nowDate = this->date->GetValue();

        res[0] = nowDate.day / 10 + 48;
        res[1] = nowDate.day % 10 + 48;

        res[3] = nowDate.month / 10 + 48;
        res[4] = nowDate.month % 10 + 48;

        int year = nowDate.year;

        res[6] = year / 1000 + 48;
        year %= 1000;

        res[7] = year / 100 + 48;
        year %= 100;

        res[8] = year / 10 + 48;
        res[9] = year % 10 + 48;

        return String(res);
    }

    void Clock::ParseFromNtp(unsigned long time)
    {
        int UTC = 3;

        // корректировка часового пояса и синхронизация
        time = time + UTC * 3600;

        int second = time % 60;
        time /= 60; // now it is minutes
        int minute = time % 60;
        time /= 60; // now it is hours
        int hour = time % 24;

        time /= 24; // now it is days
        //int dayOfWeak = (time + 4) % 7; // day week, 0-sunday
        int year = 70;

        int days = 0;
        while (days + ((year % 4) ? 365 : 366) <= (long)time)
        {
            days += (year % 4) ? 365 : 366;
            year++;
        }
        time -= days; // now it is days in this year, starting at 0

        days = 0;
        int month = 0;
        byte monthLength = 0;

        for (month = 0; month < 12; month++)
        {
            if (month == 1)
            { // february
                if (year % 4)
                {
                    monthLength = 28;
                }
                else
                {
                    monthLength = 29;
                }
            }
            else
            {
                monthLength = this->monthDays[month];
            }

            if (time >= monthLength)
            {
                time -= monthLength;
            }
            else
            {
                break;
            }
        }
        month++;            // jan is 1
        int day = time + 1; // day of month
        year = 2000 + year % 100;

        this->SetTime(hour, minute, second);
        this->SetDate(day, month, year);
    }

    void Clock::SetTimeChangeCallback(TrackedVal::Callback callback)
    {
        this->time->SetCallback(callback);
    }

    void Clock::SetDateChangeCallback(TrackedVal::Callback callback)
    {
        this->date->SetCallback(callback);
    }
}