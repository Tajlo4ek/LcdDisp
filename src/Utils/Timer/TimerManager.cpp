#include "TimerManager.h"

namespace TimerManager
{

    TimerManager::TimerManager()
    {
    }

    void TimerManager::Tick()
    {
        for (auto timer : this->timers)
        {
            timer->Tick();
        }
    }

    void TimerManager::StartAll()
    {
        for (auto timer : this->timers)
        {
            timer->Start();
        }
    }

    void TimerManager::ResumeAll()
    {
        for (auto timer : this->timers)
        {
            timer->Resume();
            timer->Tick();
        }
    }

    void TimerManager::StopAll()
    {
        for (auto timer : this->timers)
        {
            timer->Stop();
        }
    }

    void TimerManager::AddTimer(MillisTimer::Timer &timer)
    {
        this->timers.push_back(&timer);
    }

}