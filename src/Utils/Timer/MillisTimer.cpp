#include "MillisTimer.h"

namespace MillisTimer
{
    Timer::Timer()
    {
        this->callback = nullptr;
        this->enabled = false;
        this->nextCallback = 0;
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
        this->nextCallback = (enabled == false) ? 0 : millis() + this->interval;
    }

}