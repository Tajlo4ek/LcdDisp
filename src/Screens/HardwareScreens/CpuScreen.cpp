#include "CpuScreen.h"

#include "Hardwares/Hardware.h"
#include "Utils/DrawUtils/Color.h"

namespace HardwareScreens
{
    CpuScreen::CpuScreen(TFT_eSPI *lcd, BaseScreen::OnScreenWorkEnd onWorkEnd)
        : BaseScreen::Screen(lcd, onWorkEnd)
    {
        this->cpu = nullptr;

        Controls::ControlRect chartRect = {10, lcd->height() - 100 - 15, lcd->width() - 20, 100};
        this->chart = new Controls::Chart(lcd, chartRect);

        Controls::ControlRect labelRect = {2, 2, lcd->width(), 10};
        this->labelCpuName = new Controls::Label(lcd, labelRect, Controls::Label::TextSize::Small);
        this->labelCpuName->SetColor(DrawUtils::Get565Color(0, 255, 0), DrawUtils::Get565Color(0, 0, 0));

        labelRect = {2, lcd->height() - 10, lcd->width(), 10};
        this->labelCpuCurrentValue = new Controls::Label(lcd, labelRect, Controls::Label::TextSize::Small);
        this->labelCpuCurrentValue->SetColor(DrawUtils::Get565Color(0, 255, 0), DrawUtils::Get565Color(0, 0, 0));
    }

    void CpuScreen::EnterFocus()
    {
        lcd->fillScreen(DrawUtils::Get565Color(0, 0, 0));
    }

    void CpuScreen::Loop()
    {
    }

    String CpuScreen::ParseMessage(const String &message)
    {
        if (cpu == nullptr)
        {
            cpu = (Hardwares::CPU *)Hardwares::Hardware::FromJson(message, Hardwares::HardwareTypes::CPU_TYPE);
            for (int i = 0; i < cpu->GetCoreCount(); i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    Controls::ControlRect labelRect = {2, 15 + (i * 2 + j) * 12, lcd->width(), 12};
                    Controls::Label *label = new Controls::Label(lcd, labelRect, Controls::Label::TextSize::Small);
                    label->SetColor(DrawUtils::Get565Color(0, 255, 0), DrawUtils::Get565Color(0, 0, 0));
                    label->SetVisible(false);
                    labelCoreInfo.push_back(label);
                }
            }

            SetMode(ScreenMode::ChartCpuLoadMode);
        }
        else
        {
            cpu->AddData(message);
        }

        Draw();

        return "";
    }

    void CpuScreen::SetMode(ScreenMode nextMode)
    {
        screenMode = nextMode;
        if (cpu == nullptr)
        {
            return;
        }

        lcd->fillScreen(DrawUtils::Get565Color(0, 0, 0));
        labelCpuName->DrawText(cpu->GetName(), Controls::Label::TextAlignment::Center);

        switch (screenMode)
        {
        case ScreenMode::ViewCoreInfoMode:
        {
            chart->SetVisible(false);
            for (uint i = 0; i < labelCoreInfo.size(); i++)
            {
                labelCoreInfo[i]->SetVisible(true);
            }
        }
        break;

        case ScreenMode::ChartCpuTempMode:
        case ScreenMode::ChartCpuLoadMode:
        {
            chart->ResetValues();
            chart->SetVisible(true);

            for (uint i = 0; i < labelCoreInfo.size(); i++)
            {
                labelCoreInfo[i]->SetVisible(false);
            }
        }
        break;
        }

        Draw();
    }

    void CpuScreen::Draw()
    {
        if (cpu == nullptr)
        {
            return;
        }

        switch (screenMode)
        {
        case ScreenMode::ChartCpuLoadMode:
        {
            DrawChartCpuLoad();
        }
        break;
        case ScreenMode::ChartCpuTempMode:
        {
            DrawChartCpuTemp();
        }
        break;

        case ScreenMode::ViewCoreInfoMode:
        {
            ViewCoreInfo();
        }
        break;
        }
    }

    inline void CpuScreen::DrawChartCpuLoad() const
    {
        int value = cpu->GetAvgLoad();
        chart->AddValue(value);
        String text = F("now load: ");
        text += value;
        text += '%';
        labelCpuCurrentValue->DrawText(text, Controls::Label::TextAlignment::Center);
    }

    inline void CpuScreen::DrawChartCpuTemp() const
    {
        int value = cpu->GetAvgTemp();
        chart->AddValue(value);
        String text = F("now temp: ");
        text += value;
        text += 'C';
        labelCpuCurrentValue->DrawText(text, Controls::Label::TextAlignment::Center);
    }

    inline void CpuScreen::ViewCoreInfo() const
    {
        const Hardwares::CPU::CpuCore *cores = cpu->GetCores();
        for (int coreNum = 0; coreNum < cpu->GetCoreCount(); coreNum++)
        {
            String text = String(F("core #"));
            if (coreNum < 9)
            {
                text += '0';
            }
            text += coreNum + 1;
            text += F(" clock: ");
            text += cores[coreNum].clock;
            text += F(" MHz");

            labelCoreInfo[coreNum * 2]->DrawText(text, Controls::Label::TextAlignment::Left);

            text = F("load: ");
            text += cores[coreNum].load;
            text += '%';
            text += F(" temp: ");
            text += cores[coreNum].temp;
            text += 'C';

            labelCoreInfo[coreNum * 2 + 1]->DrawText(text, Controls::Label::TextAlignment::Left);
        }
    }

    void CpuScreen::ReloadConfig()
    {
    }

    CpuScreen::~CpuScreen()
    {
        delete cpu;
        delete chart;
        delete labelCpuName;

        for (uint i = 0; i < labelCoreInfo.size(); i++)
        {
            delete labelCoreInfo[i];
        }
        labelCoreInfo.clear();
    }

}