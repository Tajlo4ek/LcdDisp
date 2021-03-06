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

        enum TextSize
        {
            Small = 8,
            Big = 16,
        };

        Label(TFT_eSPI *lcd, ControlRect rect, TextSize size);
        ~Label();

        void DrawText(const String &text, TextAlignment alignment);

        void ReDraw() override;

    protected:
        String text;
        TextSize size;
        TextAlignment alignment;

        virtual void Draw();
    };

}