#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

namespace BaseScreen
{
    typedef std::function<void()> OnScreenWorkEnd;

    class Screen
    {

    protected:
        bool hasEthernet;
        TFT_eSPI *lcd;

        OnScreenWorkEnd onScreenWorkEnd = nullptr;

    public:
        virtual void EnterFocus() = 0;
        virtual void LeaveFocus() = 0;
        virtual void Loop() = 0;
        virtual String ParseMessage(const String &message) = 0;
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

        /*virtual void OnBtnUpClick() = 0;
        virtual void OnBtnDownClick() = 0;
        virtual void OnBtnLeftClick() = 0;
        virtual void OnBtnRightClick() = 0;
        virtual void OnBtnCenterClick() = 0;*/
    };
}