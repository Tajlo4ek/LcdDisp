#include "BaseScreen.h"

namespace Screens
{
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

    Screen::Screen(TFT_eSPI *lcd)
    {
        this->lcd = lcd;
    }

    void Screen::ClearScreen()
    {
        lcd->resetViewport();
        lcd->fillScreen(this->backColor);
    }

    bool Screen::OnBtnLeftClick() { return false; }
    bool Screen::OnBtnRightClick() { return false; }
    bool Screen::OnBtnCenterClick() { return false; }
}