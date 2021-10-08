#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

namespace Screens
{
    typedef std::function<void()> OnScreenWorkEnd;

    class Screen
    {
    protected:
        bool hasEthernet;
        TFT_eSPI *lcd;

    public:
        virtual void EnterFocus() = 0;
        virtual void LeaveFocus();
        virtual void Loop();
        virtual String ParseMessage(const String &message);
        virtual void ReloadConfig() = 0;

        void SetEthernetAvailable(bool val);

        Screen(TFT_eSPI *lcd);

        virtual bool OnBtnLeftClick();
        virtual bool OnBtnRightClick();
        virtual bool OnBtnCenterClick();
    };
}