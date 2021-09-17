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

        virtual void LoadDefaultConfig() = 0;

    public:
        ScreenDrawer(TFT_eSPI &lcd, int lcdWidth, int lcdHeight)
        {
            this->lcd = &lcd;
            this->lcdWidth = lcdWidth;
            this->lcdHeight = lcdHeight;
        }

        virtual void ReloadConfig() = 0;
    };
}