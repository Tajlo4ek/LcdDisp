#include "VisualizerScreen.h"

#include "Commands.h"
#include "Utils/DrawUtils/Color.h"

namespace Screens
{
    VisualizerScreen::VisualizerScreen(TFT_eSPI *lcd)
        : Screen(lcd)
    {
        Controls::ControlRect rect = {5, 0, 150, 128};
        this->visualizer = new Controls::VisualizerControl(lcd, rect);
    }

    void VisualizerScreen::ReloadConfig()
    {
        //spectrumDrawer->ReloadConfig();

        /*
        #define CONFIG_BACK_COLOR F("backColor")
#define CONFIG_LOW_COLOR F("lowColor")
#define CONFIG_MEDIUM_COLOR F("mediumColor")
#define CONFIG_HIGH_COLOR F("highColor")
#define CONFIG_MAX_COLOR F("maxColor")

auto json = FileSystem::ReadFile(SPECTRUM_CONFIG_PATH);
        if (json.isEmpty())
        {
            this->CreateDefaultConfig();
        }

        const uint colorCount = 5;
        String colorNames[colorCount]{
            CONFIG_BACK_COLOR,
            CONFIG_LOW_COLOR,
            CONFIG_MEDIUM_COLOR,
            CONFIG_HIGH_COLOR,
            CONFIG_MAX_COLOR,
        };

        uint16_t *colors[colorCount]{
            &this->backColor,
            &this->lowColor,
            &this->mediumColor,
            &this->highColor,
            &this->maxColor,
        };

        bool loadRes = DrawUtils::LoadColorsFromJson(json, colorNames, colors, colorCount);

        if (loadRes == false)
        {
            this->CreateDefaultConfig();
            this->ReloadConfig();
        }
        
        */
    }

    String VisualizerScreen::ParseMessage(const String &message)
    {
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
        data += visualizer->GetLineCount();
        data += COMMAND_STOP_CHAR;
        data += COMMAND_SET_MAX_SPECTRUM_DATA;
        data += visualizer->GetMaxLineLength();
        data += COMMAND_STOP_CHAR;
        return data;
    }

    void VisualizerScreen::ParseSpectrum(const String &data)
    {
        auto spectrumLen = this->visualizer->GetLineCount();
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
        visualizer->DrawSpectrum(spectrumLeft, spectrumRight);
        delete[] spectrumLeft;
        delete[] spectrumRight;
    }

    void VisualizerScreen::EnterFocus()
    {
        lcd->fillScreen(DrawUtils::Get565Color(0, 0, 0));
        this->visualizer->Reset();
        this->visualizer->ReDraw();
    }

    VisualizerScreen::~VisualizerScreen()
    {
        delete this->visualizer;
    }
}