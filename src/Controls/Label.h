#pragma once

#include "BaseControl.h"

namespace Controls
{
    class Label : public BaseControl
    {
    private:
        String text;
        uint16_t textColor;
        uint16_t backColor;
        byte size;
        bool isCentral;

        void DrawText(const uint16_t color);

    public:
        Label(TFT_eSPI *lcd, ControlRect rect);

        void DrawText(const String &text);
        void DrawCentralText(const String &text);
        void SetSize(const byte size);
        void SetColor(const uint16_t textColor, const uint16_t backColor);

        void ReDraw() override;
    };

}