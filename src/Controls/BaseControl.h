#pragma once

#include <TFT_eSPI.h>

namespace Controls
{
    struct ControlRect
    {
        int leftUpX;
        int leftUpY;
        int width;
        int height;
    };

    class BaseControl
    {
    protected:
        ControlRect controlRect;
        TFT_eSPI *lcd;

    public:
        BaseControl(TFT_eSPI *lcd, ControlRect controlRect)
        {
            this->lcd = lcd;
            this->controlRect = controlRect;
        }

        virtual ~BaseControl()
        {
            this->lcd = nullptr;
        }

        virtual void ReDraw() = 0;
    };

}