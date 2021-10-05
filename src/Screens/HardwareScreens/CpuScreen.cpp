#include "CpuScreen.h"

#include "Hardwares/Hardware.h"

namespace HardwareScreens
{
    CpuScreen::CpuScreen(TFT_eSPI *lcd, int lcdWidth, int lcdHeight, BaseScreen::OnScreenWorkEnd onWorkEnd)
        : BaseScreen::Screen(lcd, onWorkEnd)
    {
        this->cpu = nullptr;

        Controls::ControlRect chartRect = {lcdWidth - 120 - 5, lcdHeight - 100 - 10, 120, 100};
        this->chart = new Controls::Chart(lcd, chartRect);
    }

    void CpuScreen::EnterFocus()
    {
        //TODO: config
        lcd->fillScreen(TFT_BLACK);

        this->chart->ReDraw();
    }

    void CpuScreen::LeaveFocus()
    {
    }

    void CpuScreen::Loop()
    {
    }

    String CpuScreen::ParseMessage(const String &message)
    {
        if (cpu == nullptr)
        {
            cpu = (Hardwares::CPU *)Hardwares::Hardware::FromJson(message, Hardwares::HardwareTypes::CPU_TYPE);
        }
        else
        {
            cpu->AddData(message);
        }

        return "";
    }

    void CpuScreen::ReloadConfig()
    {
    }

    CpuScreen::~CpuScreen()
    {
    }

}