#include "DigitalClock.h"

#include "Utils/DrawUtils/Color.h"

namespace Controls
{
#define SPACE_SIZE 3

    DigitalClock::DigitalClock(TFT_eSPI *lcd, ControlRect rect)
        : BaseControl(lcd, rect)
    {
        this->mainColor = DrawUtils::Get565Color(0, 0, 255);
        this->clockSecondColor = DrawUtils::Get565Color(0, 0, 200);
        this->nowMinutes = 0;
        this->nowHours = 0;

        this->numWidth = (rect.width - SPACE_SIZE * 6) / 5;
        while (this->numWidth * 2 > rect.height)
        {
            this->numWidth--;
        }
        this->numHeight = 2 * this->numWidth;

        this->blockWidth = this->numWidth / 3;
        while (this->blockWidth % 2 == 0)
        {
            this->blockWidth--;
        }
        this->blockHeight = (this->numHeight - this->blockWidth * 2) / 2;

        this->numWidth += 2;
    }

    void DigitalClock::SetClockSecondColor(uint16_t color)
    {
        this->clockSecondColor = color;
    }

    void DigitalClock::ReDraw()
    {
        ClearRect();

        if (isVisible == false)
        {
            return;
        }

        DrawTime(nowHours, nowMinutes, false);
    }

    void DigitalClock::DrawTime(byte hours, byte minutes, bool needDots)
    {
        this->nowHours = hours;
        this->nowMinutes = minutes;

        DrawNum(hours / 10, controlRect.leftUpX + SPACE_SIZE, controlRect.leftUpY);
        DrawNum(hours % 10, controlRect.leftUpX + SPACE_SIZE * 2 + this->numWidth, controlRect.leftUpY);

        DrawNum(minutes / 10, controlRect.leftUpX + controlRect.width - (SPACE_SIZE + this->numWidth) * 2, controlRect.leftUpY);
        DrawNum(minutes % 10, controlRect.leftUpX + controlRect.width - (SPACE_SIZE + this->numWidth), controlRect.leftUpY);

        //TODO: dots
    }

    void DigitalClock::DrawNum(byte num, int x, int y) const
    {
        if (num > 9)
        {
            return;
        }

        //top
        uint16_t color = (num != 1 && num != 4) ? this->mainColor : this->backColor;
        DrawBlock(
            x + this->blockWidth / 2 + 2,
            y,
            color,
            Orientation::Horizontal);

        //top left
        color = (num != 1 && num != 2 && num != 3 && num != 7) ? this->mainColor : this->backColor;
        DrawBlock(
            x,
            y + this->blockWidth / 2 + 2,
            color,
            Orientation::Vertical);

        //top right
        color = (num != 5 && num != 6) ? this->mainColor : this->backColor;
        DrawBlock(
            x + this->blockHeight + 1,
            y + this->blockWidth / 2 + 2,
            color,
            Orientation::Vertical);

        //center
        color = (num > 1 && num != 7) ? this->mainColor : this->backColor;
        DrawBlock(
            x + this->blockWidth / 2 + 2,
            y + this->blockHeight + 1,
            color,
            Orientation::Horizontal);

        //bottom left
        color = (num == 0 || num == 2 || num == 6 || num == 8) ? this->mainColor : this->backColor;
        DrawBlock(
            x,
            y + this->blockWidth / 2 + 2 + this->blockHeight + 1,
            color,
            Orientation::Vertical);

        //bottom right
        color = (num != 2) ? this->mainColor : this->backColor;
        DrawBlock(
            x + this->blockHeight + 1,
            y + this->blockWidth / 2 + 2 + this->blockHeight + 1,
            color,
            Orientation::Vertical);

        //bottom
        color = (num != 1 && num != 4 && num != 7) ? this->mainColor : this->backColor;
        DrawBlock(
            x + this->blockWidth / 2 + 2,
            y + (this->blockHeight + 1) * 2,
            color,
            Orientation::Horizontal);
    }

    void DigitalClock::DrawBlock(int x, int y, uint16_t color, Orientation orientation) const
    {
        const int blockWidthDiv2 = this->blockWidth / 2;
        int offsetX = 0;
        int offsetY = 0;
        int signX = 0;
        int signY = 0;

        if (orientation == Orientation::Horizontal)
        {
            offsetX = x;
            offsetY = y + blockWidthDiv2 + 1;

            signX = 1;
            signY = -1;
        }
        else
        {
            offsetX = x + blockWidthDiv2 + 1;
            offsetY = y;

            signX = -1;
            signY = 1;
        }

        DrawFastLine(
            offsetX,
            offsetY,
            this->blockHeight,
            color,
            orientation);

        DrawFastLine(
            offsetX + blockWidthDiv2 * signX,
            offsetY + blockWidthDiv2 * signY,
            this->blockHeight - blockWidthDiv2 * 2,
            this->clockSecondColor,
            orientation);

        DrawFastLine(
            offsetX + blockWidthDiv2,
            offsetY + blockWidthDiv2,
            this->blockHeight - blockWidthDiv2 * 2,
            this->clockSecondColor,
            orientation);

        for (int i = 1; i < blockWidthDiv2; i++)
        {
            DrawFastLine(
                offsetX + i * signX,
                offsetY + i * signY,
                this->blockHeight - i * 2,
                color,
                orientation);

            DrawFastLine(
                offsetX + i,
                offsetY + i,
                this->blockHeight - i * 2,
                color,
                orientation);
        }
    }

    void DigitalClock::DrawFastLine(int x, int y, int len, uint16_t color, Orientation orientation) const
    {
        if (orientation == Orientation::Horizontal)
        {
            lcd->drawFastHLine(x + 1, y, len - 2, color);
            lcd->drawPixel(x + len - 1, y, this->clockSecondColor);
        }
        else
        {
            lcd->drawFastVLine(x, y + 1, len - 2, color);
            lcd->drawPixel(x, y + len - 1, this->clockSecondColor);
        }
        lcd->drawPixel(x, y, this->clockSecondColor);
    }

}