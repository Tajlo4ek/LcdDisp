#pragma once

#include "Label.h"

namespace Controls
{
    class MultilineLable : public Label
    {
    private:
        void Draw() override;

    public:
        MultilineLable(TFT_eSPI *lcd, ControlRect rect, TextSize size);
    };

}