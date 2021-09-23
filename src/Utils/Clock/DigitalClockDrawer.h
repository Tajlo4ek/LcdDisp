#pragma once

#include "BaseClockDrawer.h"

namespace ClockDrawer
{
    class DigitalClockDrawer : public BaseClockDrawer
    {

    private:
        uint16_t backColor;
        uint16_t clockMainColor;
        uint16_t clockSecondColor;

        int blockWidth;
        int blockHeight;

        int spaceDiv4 = (this->lcdHeight - this->blockWidth * 2) / 4;

        void DrawNum(int num, int x, int y) const;
        void DrawVerBlock(int x, int y, uint16_t color) const;
        void DrawHorBlock(int x, int y, uint16_t color) const;
        void DrawSpecLine(int x, int y, int x1, int y1, uint16_t mainColor, uint16_t secondColor) const;

        void DrawCentralText(const String &text, int y, uint16_t color, int textSize) const;
        void DrawString(const String &text, int x, int y, uint16_t color, int textSize) const;

        void WeatherChanged() override;
        void MessageChanged() override;
        void TimeChanged() override;
        void DateChanged() override;
        void TimeSyncChanged() override;

        void CreateDefaultConfig() override;
        void LoadConfig() override;

    public:
        DigitalClockDrawer(TFT_eSPI &lcd, int width, int height, Clock::Clock &clock);
        void Init() override;
        
    };

}