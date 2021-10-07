#pragma once

#include "BaseControl.h"

namespace Controls
{
    class Chart : public BaseControl
    {
    private:
        uint16_t mainColor;
        uint16_t secondColor;

        int valueCount;
        int *values;
        float scaleVal;

        void DrawChart();

    public:
        Chart(TFT_eSPI *lcd, ControlRect rect);
        ~Chart();

        void AddValue(const float value);
        void ResetValues();

        void ReDraw() override;
    };

}