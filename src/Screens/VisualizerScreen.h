#pragma once

#include "BaseScreen.h"

#include "Utils/Timer/MillisTimer.h"
#include "Utils/Drawers/SpectrumDrawer.h"

namespace VisualizerScreen
{
    class VisualizerScreen : public BaseScreen::Screen
    {
    private:
        Drawers::SpectrumDrawer *spectrumDrawer;
        MillisTimer::Timer spectrumCheckTimer;

        String GetSpectrumData();
        void ParseSpectrum(const String &data);

    public:
        void EnterFocus() override;
        void LeaveFocus() override;
        void Loop() override;
        String ParseMessage(const String &message) override;
        void ReloadConfig() override;

        VisualizerScreen(TFT_eSPI *lcd, int lcdWidth, int lcdHeight, BaseScreen::OnScreenWorkEnd onWorkEnd, int offTime);
    };
}