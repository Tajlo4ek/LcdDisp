#include "Label.h"

#include "Utils/DrawUtils/Color.h"

namespace Controls
{
    Label::Label(TFT_eSPI *lcd, ControlRect rect, TextSize size)
        : BaseControl(lcd, rect)
    {
        this->alignment = Controls::Label::TextAlignment::Left;
        this->text = "";
        this->size = size;
    }

    void Label::DrawText(const String &text, TextAlignment alignment)
    {
        this->text = text;
        this->alignment = alignment;
        ReDraw();
    }

    void Label::ReDraw()
    {
        ClearRect();

        if (isVisible == false)
        {
            return;
        }

        Draw();
    }

    void Label::Draw()
    {
        lcd->setTextSize((int)size / 8);
        lcd->setTextColor(mainColor, backColor);

        if ((int)this->size > controlRect.height)
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

    Label::~Label()
    {
        this->text.clear();
    }
}