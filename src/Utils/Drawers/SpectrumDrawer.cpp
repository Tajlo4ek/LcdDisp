#include "SpectrumDrawer.h"

#include "FileNames.h"

#include "Utils/FileSystem/FileSystem.h"
#include "Utils/DrawUtils/Color.h"
#include "Utils/Parsers/JsonParser.h"

namespace Drawers
{

#define SPECTRUM_MAX_PROC 45
#define LINE_SIZE 3
#define LINE_SPACE 1
#define RECT_SIZE 1

#define CONFIG_BACK_COLOR F("backColor")
#define CONFIG_LOW_COLOR F("lowColor")
#define CONFIG_MEDIUM_COLOR F("mediumColor")
#define CONFIG_HIGH_COLOR F("highColor")
#define CONFIG_MAX_COLOR F("maxColor")

    SpectrumDrawer::SpectrumDrawer(TFT_eSPI &lcd, int width, int height) : ScreenDrawer(lcd, width, height)
    {
        this->ReloadConfig();

        this->spectrumLineCount = width / (LINE_SIZE + LINE_SPACE);
        this->spectrumMaxSize = height * SPECTRUM_MAX_PROC / 100;
        while (this->spectrumMaxSize % 3 != 0)
        {
            this->spectrumMaxSize++;
        }
        this->spectrumMaxSizeDiv3 = this->spectrumMaxSize / 3;

        this->nowLeftSpectrum = new byte[this->spectrumLineCount];
        this->maxLeftSpectrumData = new byte[this->spectrumLineCount];

        this->nowRightSpectrum = new byte[this->spectrumLineCount];
        this->maxRightSpectrumData = new byte[this->spectrumLineCount];
    }

    void SpectrumDrawer::DrawSpectrum(byte *spectrumLeft, byte *spectrumRight)
    {
        int leftOffsetY = this->spectrumMaxSize + 1;
        int rightOffsetY = this->lcdHeight - this->spectrumMaxSize - 1;

        //left spectrum
        for (int i = 0; i < this->spectrumLineCount; i++)
        {
            byte nowVal = this->nowLeftSpectrum[i];
            byte nextVal = spectrumLeft[i];
            this->nowLeftSpectrum[i] = nextVal;

            //left
            if (this->maxLeftSpectrumData[i] != 0)
            {
                this->lcd->drawFastHLine(i * (LINE_SIZE + LINE_SPACE),
                                         leftOffsetY - this->maxLeftSpectrumData[i] - 1,
                                         LINE_SIZE,
                                         this->backColor);
            }

            if (this->maxLeftSpectrumData[i] < nextVal)
            {
                this->maxLeftSpectrumData[i] = nextVal;
            }
            else
            {
                if (this->maxLeftSpectrumData[i] > 0)
                {
                    this->maxLeftSpectrumData[i]--;
                }
            }

            if (nowVal >= nextVal)
            {
                //left
                this->lcd->fillRect(i * (LINE_SIZE + LINE_SPACE), leftOffsetY - nowVal, LINE_SIZE, nowVal - nextVal, this->backColor);
            }
            else
            {
                int lowSize = nextVal > this->spectrumMaxSizeDiv3 ? this->spectrumMaxSizeDiv3 : nextVal;

                //left
                this->lcd->fillRect(i * (LINE_SIZE + LINE_SPACE),
                                    leftOffsetY - lowSize,
                                    LINE_SIZE,
                                    lowSize,
                                    this->lowColor);

                if (lowSize == this->spectrumMaxSizeDiv3)
                {
                    int mediumSize = nextVal > this->spectrumMaxSizeDiv3 * 2 ? this->spectrumMaxSizeDiv3 : nextVal - lowSize;

                    //left
                    this->lcd->fillRect(i * (LINE_SIZE + LINE_SPACE),
                                        leftOffsetY - lowSize - mediumSize,
                                        LINE_SIZE,
                                        mediumSize,
                                        this->mediumColor);

                    if (mediumSize == this->spectrumMaxSizeDiv3)
                    {
                        int highSize = nextVal - this->spectrumMaxSizeDiv3 * 2;

                        //left
                        this->lcd->fillRect(i * (LINE_SIZE + LINE_SPACE),
                                            leftOffsetY - lowSize - mediumSize - highSize,
                                            LINE_SIZE,
                                            highSize,
                                            this->highColor);
                    }
                }
            }
        }

        //right spectrum
        for (int i = 0; i < this->spectrumLineCount; i++)
        {
            byte nowVal = this->nowRightSpectrum[i];
            byte nextVal = spectrumRight[i];
            this->nowRightSpectrum[i] = nextVal;

            if (this->maxRightSpectrumData[i] != 0)
            {
                this->lcd->drawFastHLine(i * (LINE_SIZE + LINE_SPACE),
                                         rightOffsetY + this->maxRightSpectrumData[i],
                                         LINE_SIZE,
                                         this->backColor);
            }

            if (this->maxRightSpectrumData[i] < nextVal)
            {
                this->maxRightSpectrumData[i] = nextVal;
            }
            else
            {
                if (this->maxRightSpectrumData[i] > 0)
                {
                    this->maxRightSpectrumData[i]--;
                }
            }

            if (nowVal >= nextVal)
            {
                this->lcd->fillRect(i * (LINE_SIZE + LINE_SPACE), rightOffsetY + nextVal, LINE_SIZE, nowVal - nextVal, this->backColor);
            }
            else
            {
                int lowSize = nextVal > this->spectrumMaxSizeDiv3 ? this->spectrumMaxSizeDiv3 : nextVal;
                this->lcd->fillRect(i * (LINE_SIZE + LINE_SPACE),
                                    rightOffsetY,
                                    LINE_SIZE,
                                    lowSize,
                                    this->lowColor);

                if (lowSize == this->spectrumMaxSizeDiv3)
                {
                    int mediumSize = nextVal > this->spectrumMaxSizeDiv3 * 2 ? this->spectrumMaxSizeDiv3 : nextVal - lowSize;
                    this->lcd->fillRect(i * (LINE_SIZE + LINE_SPACE),
                                        rightOffsetY + lowSize,
                                        LINE_SIZE,
                                        mediumSize,
                                        this->mediumColor);

                    if (mediumSize == this->spectrumMaxSizeDiv3)
                    {
                        int highSize = nextVal - this->spectrumMaxSizeDiv3 * 2;
                        this->lcd->fillRect(i * (LINE_SIZE + LINE_SPACE),
                                            rightOffsetY + lowSize + mediumSize,
                                            LINE_SIZE,
                                            highSize,
                                            this->highColor);
                    }
                }
            }
        }

        for (int i = 1; i < this->spectrumMaxSize / (RECT_SIZE + 1) + 1; i++)
        {
            //left
            this->lcd->drawFastHLine(0, leftOffsetY - i * (RECT_SIZE + 1), this->lcdWidth, this->backColor);

            //right
            this->lcd->drawFastHLine(0, rightOffsetY + i * (RECT_SIZE + 1) - 1, this->lcdWidth, this->backColor);
        }

        for (int i = 0; i < this->spectrumLineCount; i++)
        {
            //left
            this->lcd->drawFastHLine(i * (LINE_SIZE + LINE_SPACE),
                                     leftOffsetY - this->maxLeftSpectrumData[i] - 1,
                                     LINE_SIZE,
                                     this->maxColor);

            //right
            this->lcd->drawFastHLine(i * (LINE_SIZE + LINE_SPACE),
                                     rightOffsetY + this->maxRightSpectrumData[i],
                                     LINE_SIZE,
                                     this->maxColor);
        }
    }

    const int SpectrumDrawer::GetLineCount() const
    {
        return this->spectrumLineCount;
    }

    const int SpectrumDrawer::GetMaxLineLength() const
    {
        return this->spectrumMaxSize;
    }

    void SpectrumDrawer::ReloadConfig()
    {
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
    }

    void SpectrumDrawer::CreateDefaultConfig()
    {
        const uint configCount = 5;
        String configNames[configCount]{
            CONFIG_BACK_COLOR,
            CONFIG_LOW_COLOR,
            CONFIG_MEDIUM_COLOR,
            CONFIG_HIGH_COLOR,
            CONFIG_MAX_COLOR,
        };

        String datas[configCount]{
            DrawUtils::GetJsonColor(0, 0, 0),
            DrawUtils::GetJsonColor(0, 255, 0),
            DrawUtils::GetJsonColor(255, 255, 0),
            DrawUtils::GetJsonColor(255, 0, 0),
            DrawUtils::GetJsonColor(0, 255, 255),
        };

        FileSystem::WriteFile(
            SPECTRUM_CONFIG_PATH,
            JsonParser::BuildJson(configNames, datas, configCount));
    }

    void SpectrumDrawer::Reset()
    {
        memset(this->nowLeftSpectrum, 0, this->spectrumLineCount);
        memset(this->maxLeftSpectrumData, 0, this->spectrumLineCount);
        memset(this->nowRightSpectrum, 0, this->spectrumLineCount);
        memset(this->maxRightSpectrumData, 0, this->spectrumLineCount);
        this->ReDraw();
    }

    void SpectrumDrawer::ReDraw()
    {
        this->lcd->fillScreen(this->backColor);
        DrawSpectrum(this->nowLeftSpectrum, this->nowRightSpectrum);
    }

}