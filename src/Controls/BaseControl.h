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
        bool isVisible = true;

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
        void SetVisible(bool val)
        {
            isVisible = val;
            ReDraw();
        }
    };

}