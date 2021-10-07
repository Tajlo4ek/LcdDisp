#pragma once

#include "BaseControl.h"

namespace Controls
{
    class Label : public BaseControl
    {

    public:
        enum TextAlignment
        {
            Left,
            Center,
            Right
        };

        Label(TFT_eSPI *lcd, ControlRect rect);

        void DrawText(const String &text, TextAlignment alignment);
        void SetSize(const byte size);
        void SetColor(const uint16_t textColor, const uint16_t backColor);

        void ReDraw() override;

    private:
        String text;
        uint16_t textColor;
        byte size;
        TextAlignment alignment;

        void DrawText(const uint16_t color);
    };

}