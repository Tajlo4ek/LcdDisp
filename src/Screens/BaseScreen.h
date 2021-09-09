#pragma once

namespace BaseScreen
{
    class Screen
    {
    protected:
        bool hasEthernet;

    public:
        virtual void EnterFocus();
        virtual void LeaveFocus();
        virtual void Loop();

        virtual void SetEthernetAvailable(bool val);

        /*virtual void OnBtnUpClick() = 0;
        virtual void OnBtnDownClick() = 0;
        virtual void OnBtnLeftClick() = 0;
        virtual void OnBtnRightClick() = 0;
        virtual void OnBtnCenterClick() = 0;*/
    };
}