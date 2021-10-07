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
        uint16_t mainColor;

        void ClearRect();

    public:
        BaseControl(TFT_eSPI *lcd, ControlRect controlRect);

        virtual ~BaseControl();

        virtual void ReDraw() = 0;

        void SetColor(uint16_t mainColor, uint16_t backColor);

        void SetVisible(bool val);
    };

}