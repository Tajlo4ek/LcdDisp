#pragma once

#include <Arduino.h>

namespace MillisTimer
{
    typedef std::function<void()> Timercallback;

    class Timer
    {
    public:
        Timer();
        Timer(Timercallback callback, unsigned long interval);
        void Tick();

        void Start();
        void Stop();
        void Reset();

        Timercallback callback;
        void SetInterval(unsigned long);

    private:
        unsigned long interval;
        unsigned long nextCallback;
        bool enabled;
    };
}