#pragma once

#include "Screens/BaseScreen.h"
#include <TFT_eSPI.h>
#include "Hardwares/CPU.h"

#include "Controls/Chart.h"

namespace HardwareScreens
{
    class CpuScreen : public BaseScreen::Screen
    {
    private:
        Hardwares::CPU *cpu;
        Controls::Chart *chart;

    public:
        void EnterFocus() override;
        void LeaveFocus() override;
        void Loop() override;
        String ParseMessage(const String &message) override;
        void ReloadConfig() override;

        CpuScreen(TFT_eSPI *lcd, int lcdWidth, int lcdHeight, BaseScreen::OnScreenWorkEnd onWorkEnd);
        ~CpuScreen();
    };

}