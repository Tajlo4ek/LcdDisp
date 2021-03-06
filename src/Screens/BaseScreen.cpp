#include "BaseScreen.h"

namespace Screens
{
    Screen::Screen(TFT_eSPI *lcd)
    {
        this->lcd = lcd;
        isVisible = false;
    }

    void Screen::LeaveFocus() {}

    void Screen::Loop() {}

    String Screen::ParseMessage(const String &message)
    {
        return String();
    }

    void Screen::SetEthernetAvailable(bool val)
    {
        this->hasEthernet = val;
    }

    void Screen::ClearScreen()
    {
        lcd->resetViewport();
        lcd->fillScreen(this->backColor);
    }

    void Screen::SetVisible(bool isVisible)
    {
        for (const auto &control : controls)
        {
            control->SetScreenVisible(isVisible);
        }

        if (isVisible == true)
        {
            EnterFocus();
        }
        else
        {
            LeaveFocus();
        }
    }

    bool Screen::OnBtnLeftClick() { return false; }
    bool Screen::OnBtnRightClick() { return false; }
    bool Screen::OnBtnCenterClick() { return false; }
}