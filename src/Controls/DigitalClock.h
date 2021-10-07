#pragma once

#include "BaseControl.h"

namespace Controls
{
    //TODO: make not fixed size
    class DigitalClock : public BaseControl
    {
    private:
        void DrawNum(byte num, int x, int y) const;
        void DrawVerBlock(int x, int y, uint16_t color) const;
        void DrawHorBlock(int x, int y, uint16_t color) const;
        void DrawSpecLine(int x0, int y0, int x1, int y1, uint16_t mainColor, uint16_t secondColor) const;

        uint16_t clockMainColor;
        uint16_t clockSecondColor;
        uint16_t backColor;

        int blockWidth;
        int blockHeight;

        byte nowMinutes;
        byte nowHours;

    public:
        DigitalClock(TFT_eSPI *lcd, int posY);
        void ReDraw() override;

        void DrawTime(byte hours, byte minutes, bool needDots);
    };

}