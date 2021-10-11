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

        uint16_t backColor;
        uint16_t mainColor;

        void ClearRect();
        void SetViewPort();

    public:
        BaseControl(TFT_eSPI *lcd, ControlRect controlRect);

        virtual ~BaseControl();

        virtual void ReDraw() = 0;

        void SetMainColor(uint16_t color);
        void SetBackColor(uint16_t color);

        void SetVisible(bool val);
    };

}