#include "Label.h"

#include "Utils/DrawUtils/Color.h"

namespace Controls
{
    Label::Label(TFT_eSPI *lcd, ControlRect rect)
        : BaseControl(lcd, rect)
    {
        isCentral = false;
        text = "";
        size = 1;
        backColor = DrawUtils::Get565Color(0, 0, 0);
        backColor = DrawUtils::Get565Color(255, 255, 255);
    }

    void Label::DrawText(const String &text)
    {
        DrawText(backColor);
        this->text = text;
        this->isCentral = false;
        DrawText(textColor);
    }

    void Label::DrawCentralText(const String &text)
    {
        DrawText(backColor);
        this->text = text;
        this->isCentral = true;
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
        lcd->fillRect(controlRect.leftUpX, controlRect.leftUpY, controlRect.width, controlRect.height, backColor);
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

        if (isCentral == true)
        {
            lcd->drawString(text, controlRect.leftUpX + (controlRect.width - textWidth) / 2, controlRect.leftUpY);
        }
        else
        {
            lcd->drawString(buf, controlRect.leftUpX, controlRect.leftUpY);
        }
    }
}