#include "BaseHardwareScreen.h"

namespace HardwareScreens
{
    BaseHardwareScreen::BaseHardwareScreen(TFT_eSPI *lcd)
        : Screens::Screen(lcd)
    {
    }

    void BaseHardwareScreen::CreateDefaultConfig()
    {
    }

    void BaseHardwareScreen::ReloadConfig()
    {
    }

}