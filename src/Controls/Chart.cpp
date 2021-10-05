
#include "Chart.h"
#include "Utils/DrawUtils/Color.h"

#include "Utils/Logger/Logger.h"

namespace Controls
{
#define STEP_SIZE 2
#define SQUARE_SIZE 10

    Chart::Chart(TFT_eSPI *lcd, ControlRect rect)
        : BaseControl(lcd, rect)
    {
        backgroundColor = DrawUtils::Get565Color(0, 0, 0);
        mainColor = DrawUtils::Get565Color(0, 255, 0);
        secondColor = DrawUtils::Get565Color(0, 127, 0);

        this->valueCount = rect.width / STEP_SIZE + 1;
        this->values = new int[valueCount + 1];

        scaleVal = rect.height / 100.0F;

        ResetValues();
    }

    void Chart::ReDraw()
    {
        lcd->fillRect(controlRect.leftUpX, controlRect.leftUpY, controlRect.width, controlRect.height, backgroundColor);
        DrawChart();
    }

    void Chart::AddValue(float value)
    {
        DrawChart();

        if (value > 100)
        {
            value = 100;
        }
        else if (value < 0)
        {
            value = 0;
        }
        this->values[this->valueCount] = (int)(value * scaleVal);

        for (int i = 0; i < this->valueCount; i++)
        {
            this->values[i] = this->values[i + 1];
        }
    }

    void Chart::ResetValues()
    {
        for (int i = 0; i < valueCount + 1; i++)
        {
            values[i] = 0;
        }
    }

    void Chart::DrawChart()
    {
        if (visible == false)
        {
            return;
        }

        int leftUpX = controlRect.leftUpX;
        int leftUpY = controlRect.leftUpY;
        lcd->drawFastVLine(leftUpX, leftUpY, controlRect.height, secondColor);

        int nowIndex = 0;
        while (true)
        {
            int drawCount = SQUARE_SIZE / STEP_SIZE;

            int leftDownX = controlRect.leftUpX;
            int leftDownY = controlRect.height + controlRect.leftUpY;

            int partNum;
            for (partNum = nowIndex; partNum < nowIndex + drawCount; partNum++)
            {
                if (partNum + 1 >= this->valueCount)
                {
                    break;
                }

                lcd->drawLine(
                    leftDownX + partNum * STEP_SIZE,
                    leftDownY - values[partNum],
                    leftDownX + (partNum + 1) * STEP_SIZE,
                    leftDownY - values[partNum + 1],
                    backgroundColor);
            }

            lcd->drawFastVLine(leftUpX + partNum * STEP_SIZE, leftUpY, controlRect.height, secondColor);

            for (int i = 0; i < controlRect.height / SQUARE_SIZE + 1; i++)
            {
                lcd->drawFastHLine(leftUpX + nowIndex * STEP_SIZE, leftUpY + i * SQUARE_SIZE, (partNum - nowIndex) * STEP_SIZE, secondColor);
            }

            for (partNum = nowIndex; partNum < nowIndex + drawCount; partNum++)
            {
                if (partNum + 2 >= this->valueCount)
                {
                    break;
                }

                lcd->drawLine(
                    leftDownX + partNum * STEP_SIZE,
                    leftDownY - values[partNum + 1],
                    leftDownX + (partNum + 1) * STEP_SIZE,
                    leftDownY - values[partNum + 2],
                    mainColor);
            }

            nowIndex += drawCount;
            if (nowIndex >= this->valueCount)
            {
                break;
            }
        }
    }

    void Chart::SetColor(ColorConfigName colorName, uint16_t value)
    {
        switch (colorName)
        {
        case ColorConfigName::BackgroundColor:
            this->backgroundColor = value;
            break;
        }
    }

    Chart::~Chart()
    {
        delete[] values;
    }
}