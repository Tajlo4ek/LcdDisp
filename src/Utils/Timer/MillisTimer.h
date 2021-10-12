#pragma once

#include <Arduino.h>

namespace MillisTimer
{

    class Timer
    {
    public:
        typedef std::function<void()> Timercallback;

        Timer();
        void Tick();

        void Start();
        void Resume();
        void Stop();

        Timercallback callback;
        void SetInterval(unsigned long);

    private:
        unsigned long interval;
        unsigned long nextCallback;
        bool enabled;
    };
}