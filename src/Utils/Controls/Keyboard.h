#pragma once

#include "Keyboard.h"

namespace Keyboard
{
    int nowChX = 0;
    int nowChY = 0;
    int nowChAlph = 0;

    char alphabetLow[] = "1234567890abcdefghijklmnopqrstuvwxyz";
    char alphabetHigh[] = "1234567890ABCDEFGHIJKLMNOPQRSTUBWXYZ";

    void Draw(TFT_eSPI &lcd)
    {
        auto sqSize = 15;

        int x = 4;
        int y = lcd.height() - sqSize * 4 - 1;

        for (int i = 0; i < 11; i++)
        {
            lcd.drawFastVLine(x + i * sqSize, y, sqSize * 4, lcd.color565(0, 255, 0));
        }

        for (int i = 0; i < 5; i++)
        {
            lcd.drawFastHLine(x, y + i * sqSize, sqSize * 10, lcd.color565(0, 255, 0));
        }

        char *alph;
        switch (nowChAlph)
        {
        case 1:
            alph = alphabetHigh;
            break;

        default:
            nowChAlph = 0;
            alph = alphabetLow;
            break;
        }

        for (int i = 0; i < 36; i++)
        {
            String str = String(alph[i]);
            int strLen = lcd.textWidth(str, 1);
            lcd.drawString(str, x + (i % 10) * sqSize + (sqSize - strLen) / 2 + 1, y + (i / 10) * sqSize + 4, 1, lcd.color565(0, 255, 0));
        }
    }


    /* #region  Move */
    void MoveLeft()
    {
        nowChX--;
        if (nowChX < 0)
        {
            nowChX = 0;
        }
    }

    void MoveRight()
    {
        nowChX++;
        if (nowChX > 9)
        {
            nowChX = 9;
        }
    }

    void MoveUp()
    {
        nowChY--;
        if (nowChY < 0)
        {
            nowChY = 0;
        }
    }

    void MoveDown()
    {
        nowChY++;
        if (nowChY > 3)
        {
            nowChY = 3;
        }
    }
    /* #endregion */

}