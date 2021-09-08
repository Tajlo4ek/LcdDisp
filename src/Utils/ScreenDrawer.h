#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

namespace ScreenDrawer
{
    class ScreenDrawer
    {
    protected:
        TFT_eSPI *lcd;
        int lcdWidth;
        int lcdHeight;

    public:
        ScreenDrawer(TFT_eSPI &lcd, int lcdWidth, int lcdHeight)
        {
            this->lcd = &lcd;
            this->lcdWidth = lcdWidth;
            this->lcdHeight = lcdHeight;
        }
    };
}