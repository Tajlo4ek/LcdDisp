#pragma once

#include <Arduino.h>
#include "MillisTimer.h"

#include <list>

namespace TimerManager
{

    class TimerManager
    {
    public:
        TimerManager();

        void Tick();

        void StartAll();
        void ResumeAll();
        void StopAll();

        void AddTimer(MillisTimer::Timer &timer);

    private:
        std::list<MillisTimer::Timer *> timers;
    };
}