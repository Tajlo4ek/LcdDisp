#include "Label.h"

#include "Utils/DrawUtils/Color.h"

namespace Controls
{
    Label::Label(TFT_eSPI *lcd, ControlRect rect)
        : BaseControl(lcd, rect)
    {
        this->alignment = Controls::Label::TextAlignment::Left;
        this->text = "";
        this->size = 1;
    }

    void Label::DrawText(const String &text, TextAlignment alignment)
    {
        DrawText(backColor);
        this->text = text;
        this->alignment = alignment;
        DrawText(textColor);
    }

    void Label::SetColor(const uint16_t textColor, const uint16_t backColor)
    {
        this->textColor = textColor;
        this->backColor = backColor;
        ReDraw();
    }

    void Label::SetSize(const byte size)
    {
        DrawText(backColor);
        this->size = size;
        DrawText(textColor);
    }

    void Label::ReDraw()
    {
        ClearRect();
        DrawText(textColor);
    }

    void Label::DrawText(const uint16_t color)
    {
        if (isVisible == false)
        {
            return;
        }

        lcd->setTextSize(size);
        lcd->setTextColor(textColor, backColor);

        if (size * 8 > controlRect.height)
        {
            lcd->setTextSize(1);
            lcd->drawString(F("size to big"), controlRect.leftUpX, controlRect.leftUpY);
            return;
        }

        String buf = text;
        int textWidth = lcd->textWidth(text);
        while (textWidth > controlRect.width)
        {
            buf.remove(buf.length() - 1);
            textWidth = lcd->textWidth(buf);
        }

        switch (this->alignment)
        {
        default:
        case TextAlignment::Left:
            lcd->drawString(buf, controlRect.leftUpX, controlRect.leftUpY);
            break;

        case TextAlignment::Center:
            lcd->drawString(text, controlRect.leftUpX + (controlRect.width - textWidth) / 2, controlRect.leftUpY);
            break;

        case TextAlignment::Right:
            lcd->drawString(buf, controlRect.leftUpX + controlRect.width - textWidth, controlRect.leftUpY);
            break;
        }
    }
}