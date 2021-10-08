#include "MultilineLable.h"

#include "Utils/Logger/Logger.h"

namespace Controls
{
    MultilineLable::MultilineLable(TFT_eSPI *lcd, ControlRect rect, TextSize size)
        : Label(lcd, rect, size)
    {
    }

    void MultilineLable::Draw()
    {
        lcd->setTextSize((int)size / 8);
        lcd->setTextColor(mainColor, backColor);

        int nowY = controlRect.leftUpY;

        String text = this->text + ' ';
        int nowIndex = 0;
        while (nowY < controlRect.leftUpY + controlRect.height)
        {
            String buf;

            while (true)
            {
                int ind = text.indexOf(' ', nowIndex);
                if (ind == -1)
                {
                    break;
                }

                String sub = text.substring(nowIndex, ind);

                if (lcd->textWidth(buf) + lcd->textWidth(sub) >= controlRect.width)
                {
                    if (buf.isEmpty() == true)
                    {
                        buf = sub;
                    }
                    break;
                }

                buf += sub;
                buf += ' ';
                nowIndex = ind + 1;
            }

            lcd->drawString(buf, controlRect.leftUpX, nowY);

            nowY += (int)size;
        }
    }

}