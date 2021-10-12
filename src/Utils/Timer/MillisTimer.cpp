#include "MillisTimer.h"

namespace MillisTimer
{
    Timer::Timer() : Timer(nullptr, 0)
    {
    }

    Timer::Timer(Timercallback callback, int interval)
    {
        this->callback = callback;
        this->enabled = false;
        this->nextCallback = 0;
        this->interval = interval;
    }

    void Timer::Tick()
    {
        if (this->enabled == false)
        {
            return;
        }

        if (millis() >= this->nextCallback)
        {
            this->enabled = false;
            this->callback();

            if (this->interval != 0)
            {
                this->nextCallback = millis() + this->interval;
                this->enabled = true;
            }
        }
    }

    void Timer::Start()
    {
        this->nextCallback = millis() + this->interval;
        Resume();
    }

    void Timer::Resume()
    {
        this->enabled = true;
    }

    void Timer::Stop()
    {
        this->enabled = false;
    }

    void Timer::SetInterval(unsigned long val)
    {
        this->interval = val;
        this->nextCallback = millis() + this->interval;
    }

}