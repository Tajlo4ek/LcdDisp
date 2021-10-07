#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

namespace BaseScreen
{
    typedef std::function<void()> OnScreenWorkEnd;

    class Screen
    {

    private:
        OnScreenWorkEnd onScreenWorkEnd = nullptr;

    protected:
        bool hasEthernet;
        TFT_eSPI *lcd;

    public:
        virtual void EnterFocus() = 0;
        virtual void LeaveFocus()
        {
            if (onScreenWorkEnd != nullptr)
            {
                onScreenWorkEnd();
            }
        };
        virtual void Loop() = 0;
        virtual String ParseMessage(const String &message) { return String(); }
        virtual void ReloadConfig() = 0;

        void SetEthernetAvailable(bool val)
        {
            this->hasEthernet = val;
        }

        Screen(TFT_eSPI *lcd, OnScreenWorkEnd onScreenWorkEnd)
        {
            this->onScreenWorkEnd = onScreenWorkEnd;
            this->lcd = lcd;
        }

        virtual bool OnBtnLeftClick() { return false; }
        virtual bool OnBtnRightClick() { return false; }
        virtual bool OnBtnCenterClick() { return false; }
    };
}