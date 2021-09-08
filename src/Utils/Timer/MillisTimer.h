#pragma once

#include <Arduino.h>

namespace MillisTimer
{
    typedef void (*Timercallback)();

    class Timer
    {
    public:
        Timer(Timercallback callback, unsigned long interval);
        void Tick();

        void Start();
        void Stop();

        Timercallback callback;

    private:
        unsigned long interval;
        unsigned long nextCallback;
        bool enabled;
    };
}