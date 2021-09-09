#include "MillisTimer.h"

namespace MillisTimer
{
    Timer::Timer()
    {
        this->callback = nullptr;
        this->interval = -1;
        this->enabled = false;
    }

    Timer::Timer(Timercallback callback, unsigned long interval)
    {
        this->callback = callback;
        this->interval = interval;
        this->enabled = false;
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
            callback();
            this->nextCallback = millis() + this->interval;
            this->enabled = true;
        }
    }

    void Timer::Start()
    {
        this->enabled = true;
        this->nextCallback = millis() + this->interval;
    }

    void Timer::Stop()
    {
        this->enabled = false;
    }

    void Timer::SetInterval(unsigned long val)
    {
        this->interval = val;
    }

}