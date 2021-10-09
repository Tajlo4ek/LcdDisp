#pragma once

#include "Screens/BaseScreen.h"

namespace HardwareScreens
{
    class BaseHardwareScreen : public Screens::Screen
    {
    private:
        void CreateDefaultConfig() override;

    public:
        BaseHardwareScreen(TFT_eSPI *lcd);
        void ReloadConfig() override;
    };

}