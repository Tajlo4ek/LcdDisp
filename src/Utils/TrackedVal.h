#pragma once

#include <Arduino.h>

namespace TrackedVal
{
    typedef std::function<void()> Callback;

    template <typename T>
    class TrackedValue
    {

    public:
        TrackedValue(T val)
        {
            this->currentVal = val;
            this->onValueChange = nullptr;
            this->isCallbackSet = false;
        }

        TrackedValue(T val, Callback onValueChange)
        {
            this->currentVal = val;
            this->SetCallback(onValueChange);
        }

        void SetCallback(Callback onValueChange)
        {
            this->onValueChange = onValueChange;
            this->isCallbackSet = true;
        }

        void SetValue(T val)
        {
            this->prevVal = this->currentVal;
            this->currentVal = val;

            if (this->isCallbackSet)
            {
                onValueChange();
            }
        }

        T GetCurrentValue()
        {
            return this->currentVal;
        }

        T GetPrevievValue()
        {
            return this->prevVal;
        }

        ~TrackedValue()
        {
            this->onValueChange = nullptr;
        }

    private:
        TrackedValue() = delete;

        T currentVal;
        T prevVal;
        Callback onValueChange;
        bool isCallbackSet;
    };

}