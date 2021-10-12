#include "TimerManager.h"

namespace TimerManager
{

    TimerManager::TimerManager()
    {
    }

    void TimerManager::Tick()
    {
        for (const auto &timer : this->timers)
        {
            timer->Tick();
        }
    }

    void TimerManager::StartAll()
    {
        for (const auto &timer : this->timers)
        {
            timer->Start();
        }
    }

    void TimerManager::ResumeAll()
    {
        for (const auto &timer : this->timers)
        {
            timer->Resume();
            timer->Tick();
        }
    }

    void TimerManager::StopAll()
    {
        for (const auto &timer : this->timers)
        {
            timer->Stop();
        }
    }

    void TimerManager::AddTimer(MillisTimer::Timer &timer)
    {
        this->timers.push_back(&timer);
    }

}