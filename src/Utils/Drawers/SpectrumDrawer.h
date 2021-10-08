#pragma once

#include "ScreenDrawer.h"

namespace Drawers
{
    class SpectrumDrawer : public ScreenDrawer::ScreenDrawer
    {
    public:
        SpectrumDrawer(TFT_eSPI *lcd, int width, int height);

        void DrawSpectrum(byte *spectrumLeft, byte *spectrumRight);
        const int GetLineCount() const;
        const int GetMaxLineLength() const;

        const unsigned long GetLastUpdateTime() const;
        void Reset();

        void ReloadConfig() override;

        void ReDraw() override;

    private:
        int spectrumLineCount;
        byte *nowLeftSpectrum;
        byte *maxLeftSpectrumData;

        byte *nowRightSpectrum;
        byte *maxRightSpectrumData;

        int spectrumMaxSize;
        int spectrumMaxSizeDiv3;

        uint16_t backColor;
        uint16_t lowColor;
        uint16_t mediumColor;
        uint16_t highColor;
        uint16_t maxColor;

        void CreateDefaultConfig() override;
    };
}