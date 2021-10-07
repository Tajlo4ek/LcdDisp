#include "DigitalClock.h"

#include "Utils/DrawUtils/Color.h"

namespace Controls
{
#define SPACE_SIZE 3

    DigitalClock::DigitalClock(TFT_eSPI *lcd, int posY)
        : BaseControl(lcd, {0,
                            posY,
                            lcd->width(),
                            63})
    {
        this->clockMainColor = DrawUtils::Get565Color(0, 0, 255);
        this->clockSecondColor = DrawUtils::Get565Color(0, 0, 200);

        this->blockWidth = 29;
        this->blockHeight = 9;
    }

    void DigitalClock::ReDraw()
    {
        lcd->fillRect(
            controlRect.leftUpX,
            controlRect.leftUpY,
            controlRect.width,
            controlRect.height,
            DrawUtils::Get565Color(0, 0, 0));

        if (isVisible == false)
        {
            return;
        }

        DrawTime(nowHours, nowMinutes, false);
    }

    void DigitalClock::DrawTime(byte hours, byte minutes, bool needDots)
    {
        this->nowMinutes = minutes;
        this->nowHours = hours;

        DrawNum(hours / 10, SPACE_SIZE, controlRect.leftUpY);
        DrawNum(hours % 10, SPACE_SIZE * 2 + this->blockWidth, controlRect.leftUpY);

        DrawNum(minutes / 10, controlRect.width - (SPACE_SIZE + this->blockWidth) * 2, controlRect.leftUpY);
        DrawNum(minutes % 10, controlRect.width - SPACE_SIZE - this->blockWidth, controlRect.leftUpY);

        //dot radius = 30% of empty central space
        int delta = controlRect.width - (SPACE_SIZE + this->blockWidth) * 4;
        int dotRadius = delta * 30 / 100 / 2;

        int dotX = controlRect.width / 2;
        int dotY = (this->blockWidth - dotRadius) / 2;

        uint16_t dotColor = needDots ? this->clockMainColor : this->backColor;
        this->lcd->fillEllipse(dotX, controlRect.leftUpY + dotY, dotRadius, dotRadius, dotColor);
        this->lcd->fillEllipse(dotX, controlRect.leftUpY + dotY + this->blockWidth - this->blockHeight, dotRadius, dotRadius, dotColor);
    }

    void DigitalClock::DrawNum(byte num, int x, int y) const
    {
        if (num > 9)
        {
            num = 0;
        }

        //top
        if (num != 1 && num != 4)
        {
            DrawHorBlock(x, y, this->clockMainColor);
        }
        else
        {
            DrawHorBlock(x, y, this->backColor);
        }

        //left top
        if (num != 1 && num != 2 && num != 3 && num != 7)
        {
            DrawVerBlock(x, y, this->clockMainColor);
        }
        else
        {
            DrawVerBlock(x, y, this->backColor);
        }

        //right top
        if (num != 5 && num != 6)
        {
            DrawVerBlock(x + this->blockWidth - this->blockHeight + 1, y, this->clockMainColor);
        }
        else
        {
            DrawVerBlock(x + this->blockWidth - this->blockHeight + 1, y, this->backColor);
        }

        //center
        if (num > 1 && num != 7)
        {
            DrawHorBlock(x, y + this->blockWidth - this->blockHeight + 1, this->clockMainColor);
        }
        else
        {
            DrawHorBlock(x, y + this->blockWidth - this->blockHeight + 1, this->backColor);
        }

        //left bottom
        if (num == 0 || num == 2 || num == 6 || num == 8)
        {
            DrawVerBlock(x, y + this->blockWidth - this->blockHeight + 1, this->clockMainColor);
        }
        else
        {
            DrawVerBlock(x, y + this->blockWidth - this->blockHeight + 1, this->backColor);
        }

        //right bottom
        if (num != 2)
        {
            DrawVerBlock(x + this->blockWidth - this->blockHeight + 1, y + this->blockWidth - this->blockHeight + 1, this->clockMainColor);
        }
        else
        {
            DrawVerBlock(x + this->blockWidth - this->blockHeight + 1, y + this->blockWidth - this->blockHeight + 1, this->backColor);
        }

        //bottom
        if (num != 1 && num != 4 && num != 7)
        {
            DrawHorBlock(x, y + (this->blockWidth - this->blockHeight + 1) * 2, this->clockMainColor);
        }
        else
        {
            DrawHorBlock(x, y + (this->blockWidth - this->blockHeight + 1) * 2, this->backColor);
        }
    }

    void DigitalClock::DrawVerBlock(int x, int y, uint16_t color) const
    {
        int blockHeightDiv2 = this->blockHeight / 2;

        this->DrawSpecLine(x + blockHeightDiv2,
                           y + 1 + blockHeightDiv2,
                           x + blockHeightDiv2,
                           y + blockWidth - 1 - blockHeightDiv2,
                           color,
                           this->clockSecondColor);

        for (int i = 1; i < blockHeightDiv2; i++)
        {
            this->DrawSpecLine(x + blockHeightDiv2 + i,
                               y + 1 + i + blockHeightDiv2,
                               x + blockHeightDiv2 + i,
                               y + blockWidth - 1 - i - blockHeightDiv2,
                               i == blockHeightDiv2 - 1 ? this->clockSecondColor : color,
                               this->clockSecondColor);

            this->DrawSpecLine(x + blockHeightDiv2 - i,
                               y + 1 + i + blockHeightDiv2,
                               x + blockHeightDiv2 - i,
                               y + blockWidth - 1 - i - blockHeightDiv2,
                               i == blockHeightDiv2 - 1 ? this->clockSecondColor : color,
                               this->clockSecondColor);
        }
    }

    void DigitalClock::DrawHorBlock(int x, int y, uint16_t color) const
    {
        int blockHeightDiv2 = this->blockHeight / 2;

        this->DrawSpecLine(x + 1 + blockHeightDiv2,
                           y + blockHeightDiv2,
                           x + blockWidth - 1 - blockHeightDiv2,
                           y + blockHeightDiv2,
                           color,
                           this->clockSecondColor);

        for (int i = 1; i < blockHeightDiv2; i++)
        {
            this->DrawSpecLine(x + 1 + i + blockHeightDiv2,
                               y + blockHeightDiv2 + i,
                               x + blockWidth - 1 - i - blockHeightDiv2,
                               y + blockHeightDiv2 + i,
                               i == blockHeightDiv2 - 1 ? this->clockSecondColor : color,
                               this->clockSecondColor);

            this->DrawSpecLine(x + 1 + i + blockHeightDiv2,
                               y + blockHeightDiv2 - i,
                               x + blockWidth - 1 - i - blockHeightDiv2,
                               y + blockHeightDiv2 - i,
                               i == blockHeightDiv2 - 1 ? this->clockSecondColor : color,
                               this->clockSecondColor);
        }
    }

    void DigitalClock::DrawSpecLine(int x0, int y0, int x1, int y1, uint16_t mainColor, uint16_t secondColor) const
    {
        this->lcd->drawLine(x0, y0, x1, y1, mainColor);
        if (mainColor != secondColor)
        {
            this->lcd->drawPixel(x0, y0, secondColor);
            this->lcd->drawPixel(x1, y1, secondColor);
        }
    }

}