#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

namespace Drawers
{

    class ScreenDrawer
    {
    protected:
        TFT_eSPI *lcd;
        int lcdWidth;
        int lcdHeight;

        virtual void CreateDefaultConfig() = 0;

    public:
        ScreenDrawer(TFT_eSPI *lcd, int lcdWidth, int lcdHeight)
        {
            this->lcd = lcd;
            this->lcdWidth = lcdWidth;
            this->lcdHeight = lcdHeight;
        }

        virtual void ReloadConfig() = 0;
        virtual void ReDraw() = 0;

        virtual ~ScreenDrawer()
        {
            lcd = nullptr;
        }
    };
}