#include "BaseControl.h"

namespace Controls
{
    BaseControl::BaseControl(TFT_eSPI *lcd, ControlRect controlRect)
    {
        this->lcd = lcd;
        this->controlRect = controlRect;
        this->backColor = DrawUtils::Get565Color(0, 0, 0);
    }

    BaseControl::~BaseControl()
    {
        this->lcd = nullptr;
    }

    void BaseControl::SetVisible(bool val)
    {
        if (isVisible == val)
        {
            return;
        }

        isVisible = val;
        ReDraw();
    }

    void BaseControl::ClearRect()
    {
        lcd->fillRect(controlRect.leftUpX, controlRect.leftUpY, controlRect.width, controlRect.height, backColor);
    }
}