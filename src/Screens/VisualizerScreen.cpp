#include "VisualizerScreen.h"

#include "Utils/Drawers/SpectrumDrawer.h"
#include "Commands.h"

namespace VisualizerScreen
{
    VisualizerScreen::VisualizerScreen(TFT_eSPI &lcd, int lcdWidth, int lcdHeight, BaseScreen::OnScreenWorkEnd onWorkEnd, int offTime)
        : BaseScreen::Screen(onWorkEnd)
    {
        this->spectrumCheckTimer.callback = [this]()
        {
            this->onScreenWorkEnd();
        };
        this->spectrumCheckTimer.SetInterval(offTime);
        this->spectrumCheckTimer.Stop();

        this->spectrumDrawer = new Drawers::SpectrumDrawer(lcd, lcdWidth, lcdHeight);
    }

    void VisualizerScreen::ReloadConfig()
    {
        spectrumDrawer->ReloadConfig();
    }

    String VisualizerScreen::ParseMessage(const String &message)
    {
        //TODO: smt crash 
        if (message.startsWith(COMMAND_SET_MODE_SPECTRUM))
        {
            return GetSpectrumData();
        }
        else if (message.startsWith(COMMAND_SEND_SPECTRUM_DATA))
        {
            ParseSpectrum(message);
        }
        return String();
    }

    String VisualizerScreen::GetSpectrumData()
    {
        String data = COMMAND_SEND_LINE_COUNT;
        data += spectrumDrawer->GetLineCount();
        data += COMMAND_STOP_CHAR;
        data += COMMAND_SET_MAX_SPECTRUM_DATA;
        data += spectrumDrawer->GetMaxLineLength();
        data += COMMAND_STOP_CHAR;
        return data;
    }

    void VisualizerScreen::ParseSpectrum(const String &data)
    {
        this->spectrumCheckTimer.Reset();

        auto spectrumLen = this->spectrumDrawer->GetLineCount();
        auto spectrumLeft = new byte[spectrumLen];
        auto spectrumRight = new byte[spectrumLen];
        int dateLen = data.length();

        byte next = 0;
        int spNum = 0;
        for (int pos = String(COMMAND_SEND_SPECTRUM_DATA).length(); pos < dateLen; pos++)
        {
            char ch = data[pos];
            if (ch != COMMAND_SPLIT_CHAR)
            {
                next *= 10;
                next += ch - '0';
            }
            else
            {
                if (spNum < spectrumLen)
                {
                    spectrumLeft[spNum] = next;
                }
                else
                {
                    spectrumRight[spNum - spectrumLen] = next;
                }
                next = 0;
                spNum++;
            }
        }
        spectrumDrawer->DrawSpectrum(spectrumLeft, spectrumRight);
        delete[] spectrumLeft;
        delete[] spectrumRight;
    }

    void VisualizerScreen::EnterFocus()
    {
        this->spectrumDrawer->Reset();
        this->spectrumCheckTimer.Start();
    }

    void VisualizerScreen::LeaveFocus()
    {
        this->spectrumCheckTimer.Stop();
    }

    void VisualizerScreen::Loop()
    {
        this->spectrumCheckTimer.Tick();
    }
}