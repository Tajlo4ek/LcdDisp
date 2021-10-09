#pragma once

#include "BaseHardwareScreen.h"
#include <TFT_eSPI.h>
#include "Hardwares/CPU.h"

#include "Controls/Chart.h"
#include "Controls/Label.h"

namespace HardwareScreens
{
    //TODO: if core count >4
    class CpuScreen : public BaseHardwareScreen
    {
        enum ScreenMode
        {
            ChartCpuLoadMode,
            ChartCpuTempMode,
            ViewCoreInfoMode,
        };

    private:
        Hardwares::CPU *cpu;
        Controls::Chart *chart;
        Controls::Label *labelCpuName;
        Controls::Label *labelCpuCurrentValue;
        std::vector<Controls::Label *> labelCoreInfo;

        ScreenMode screenMode;

        void Draw();
        void SetMode(ScreenMode nextMode);
        inline void DrawChartCpuLoad() const;
        inline void DrawChartCpuTemp() const;
        inline void ViewCoreInfo() const;

    public:
        void EnterFocus() override;
        void Loop() override;
        String ParseMessage(const String &message) override;

        void ReDraw() override;

        CpuScreen(TFT_eSPI *lcd);
        ~CpuScreen();
    };

}