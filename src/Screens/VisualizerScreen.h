#pragma once

#include "BaseScreen.h"

#include "Utils/Timer/MillisTimer.h"
#include "Utils/Spectrum/SpectrumDrawer.h"

namespace VisualizerScreen
{
    class VisualizerScreen : public BaseScreen::Screen
    {
    private:
        SpectrumDrawer::SpectrumDrawer *spectrumDrawer;
        MillisTimer::Timer spectrumCheckTimer;

        String GetSpectrumData();
        void ParseSpectrum(String &data);

    public:
        void EnterFocus() override;
        void LeaveFocus() override;
        void Loop() override;
        String ParseMessage(String message) override;

        VisualizerScreen(TFT_eSPI &lcd, int lcdWidth, int lcdHeight, BaseScreen::OnScreenWorkEnd onWorkEnd, int offTime);
        ~VisualizerScreen();
    };
}