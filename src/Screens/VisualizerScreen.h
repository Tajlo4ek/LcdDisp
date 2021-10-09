#pragma once

#include "BaseScreen.h"

#include "Utils/Timer/MillisTimer.h"

#include "Controls/VisualizerControl.h"

namespace Screens
{
    class VisualizerScreen : public Screen
    {
    private:
        String GetSpectrumData();
        void ParseSpectrum(const String &data);

        Controls::VisualizerControl *visualizer;

        uint16_t lowColor;
        uint16_t mediumColor;
        uint16_t highColor;
        uint16_t maxColor;

        void CreateDefaultConfig() override;

    public:
        void EnterFocus() override;
        String ParseMessage(const String &message) override;
        void ReloadConfig() override;

        VisualizerScreen(TFT_eSPI *lcd);
        ~VisualizerScreen();
    };
}