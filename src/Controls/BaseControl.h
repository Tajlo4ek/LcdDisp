#pragma once

#include <TFT_eSPI.h>
#include "Utils/DrawUtils/Color.h"

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

        uint16_t backColor;

        void ClearRect();

    public:
        BaseControl(TFT_eSPI *lcd, ControlRect controlRect);

        virtual ~BaseControl();

        virtual void ReDraw() = 0;

        void SetVisible(bool val);
    };

}