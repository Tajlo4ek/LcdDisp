#include "VisualizerScreen.h"

#include "Utils/Spectrum/SpectrumDrawer.h"
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

        this->spectrumDrawer = new SpectrumDrawer::SpectrumDrawer(lcd, lcdWidth, lcdHeight);
    }

    String VisualizerScreen::ParseMessage(const String &message)
    {
        if (message.startsWith(Commands::setModeSpectrum))
        {
            return GetSpectrumData();
        }
        else if (message.startsWith(Commands::sendSpectrumData))
        {
            ParseSpectrum(message);
        }
        return String();
    }

    String VisualizerScreen::GetSpectrumData()
    {
        String data = Commands::setLineCount;
        data += spectrumDrawer->GetLineCount();
        data += Commands::stopChar;
        data += Commands::setMaxData;
        data += spectrumDrawer->GetMaxLineLength();
        data += Commands::stopChar;
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
        for (int pos = Commands::sendSpectrumData.length(); pos < dateLen; pos++)
        {
            char ch = data[pos];
            if (ch != Commands::splitChar)
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

    VisualizerScreen::~VisualizerScreen()
    {
        delete this->spectrumDrawer;
    }
}