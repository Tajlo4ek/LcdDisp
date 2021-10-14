#include "VisualizerControl.h"

#include "Utils/DrawUtils/Color.h"

namespace Controls
{

#define SPECTRUM_MAX_PROC 90
#define LINE_WIDTH 3
#define LINE_SPACE 1
#define RECT_HEIGHT 1

    VisualizerControl::VisualizerControl(TFT_eSPI *lcd, ControlRect rect)
        : BaseControl(lcd, rect)
    {
        this->lowColor = DrawUtils::Get565Color(0, 255, 0);
        this->mediumColor = DrawUtils::Get565Color(255, 255, 0);
        this->highColor = DrawUtils::Get565Color(255, 0, 0);
        this->maxColor = DrawUtils::Get565Color(0, 255, 255);

        this->spectrumLineCount = rect.width / (LINE_WIDTH + LINE_SPACE);
        this->spectrumMaxSize = rect.height * SPECTRUM_MAX_PROC / 100 / 2;
        while (this->spectrumMaxSize % 3 != 0)
        {
            this->spectrumMaxSize--;
        }
        this->spectrumMaxSizeDiv3 = this->spectrumMaxSize / 3;

        this->nowLeftSpectrum = new byte[this->spectrumLineCount];
        this->maxLeftSpectrumData = new byte[this->spectrumLineCount];

        this->nowRightSpectrum = new byte[this->spectrumLineCount];
        this->maxRightSpectrumData = new byte[this->spectrumLineCount];

        Reset();
    }

    void VisualizerControl::SetColors(uint16_t lowColor, uint16_t mediumColor, uint16_t highColor, uint16_t maxColor)
    {
        this->lowColor = lowColor;
        this->mediumColor = mediumColor;
        this->highColor = highColor;
        this->maxColor = maxColor;
    }

    void VisualizerControl::DrawSpectrum(byte *spectrumLeft, byte *spectrumRight)
    {
        if (isVisible == false || isScreenVisible == false)
        {
            for (int i = 0; i < this->spectrumLineCount; i++)
            {
                this->nowLeftSpectrum[i] = spectrumLeft[i];
                this->nowRightSpectrum[i] = spectrumRight[i];
            }
            return;
        }

        this->SetViewPort();

        int leftOffsetY = this->spectrumMaxSize + 1;
        int rightOffsetY = controlRect.height - this->spectrumMaxSize - 1;

        //left spectrum
        for (int i = 0; i < this->spectrumLineCount; i++)
        {
            byte nowVal = this->nowLeftSpectrum[i];
            byte nextVal = spectrumLeft[i];
            this->nowLeftSpectrum[i] = nextVal;

            //left
            if (this->maxLeftSpectrumData[i] != 0)
            {
                this->lcd->drawFastHLine(
                    i * (LINE_WIDTH + LINE_SPACE),
                    leftOffsetY - this->maxLeftSpectrumData[i] - 1,
                    LINE_WIDTH,
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
                this->lcd->fillRect(
                    i * (LINE_WIDTH + LINE_SPACE),
                    leftOffsetY - nowVal,
                    LINE_WIDTH,
                    nowVal - nextVal,
                    this->backColor);
            }
            else
            {
                int lowSize = nextVal > this->spectrumMaxSizeDiv3 ? this->spectrumMaxSizeDiv3 : nextVal;

                //left
                this->lcd->fillRect(
                    i * (LINE_WIDTH + LINE_SPACE),
                    leftOffsetY - lowSize,
                    LINE_WIDTH,
                    lowSize,
                    this->lowColor);

                if (lowSize == this->spectrumMaxSizeDiv3)
                {
                    int mediumSize = nextVal > this->spectrumMaxSizeDiv3 * 2 ? this->spectrumMaxSizeDiv3 : nextVal - lowSize;

                    //left
                    this->lcd->fillRect(
                        i * (LINE_WIDTH + LINE_SPACE),
                        leftOffsetY - lowSize - mediumSize,
                        LINE_WIDTH,
                        mediumSize,
                        this->mediumColor);

                    if (mediumSize == this->spectrumMaxSizeDiv3)
                    {
                        int highSize = nextVal - this->spectrumMaxSizeDiv3 * 2;

                        //left
                        this->lcd->fillRect(
                            i * (LINE_WIDTH + LINE_SPACE),
                            leftOffsetY - lowSize - mediumSize - highSize,
                            LINE_WIDTH,
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
                this->lcd->drawFastHLine(
                    i * (LINE_WIDTH + LINE_SPACE),
                    rightOffsetY + this->maxRightSpectrumData[i],
                    LINE_WIDTH,
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
                this->lcd->fillRect(
                    i * (LINE_WIDTH + LINE_SPACE),
                    rightOffsetY + nextVal,
                    LINE_WIDTH,
                    nowVal - nextVal,
                    this->backColor);
            }
            else
            {
                int lowSize = nextVal > this->spectrumMaxSizeDiv3 ? this->spectrumMaxSizeDiv3 : nextVal;
                this->lcd->fillRect(
                    i * (LINE_WIDTH + LINE_SPACE),
                    rightOffsetY,
                    LINE_WIDTH,
                    lowSize,
                    this->lowColor);

                if (lowSize == this->spectrumMaxSizeDiv3)
                {
                    int mediumSize = nextVal > this->spectrumMaxSizeDiv3 * 2 ? this->spectrumMaxSizeDiv3 : nextVal - lowSize;
                    this->lcd->fillRect(
                        i * (LINE_WIDTH + LINE_SPACE),
                        rightOffsetY + lowSize,
                        LINE_WIDTH,
                        mediumSize,
                        this->mediumColor);

                    if (mediumSize == this->spectrumMaxSizeDiv3)
                    {
                        int highSize = nextVal - this->spectrumMaxSizeDiv3 * 2;
                        this->lcd->fillRect(
                            i * (LINE_WIDTH + LINE_SPACE),
                            rightOffsetY + lowSize + mediumSize,
                            LINE_WIDTH,
                            highSize,
                            this->highColor);
                    }
                }
            }
        }

        for (int i = 1; i < this->spectrumMaxSize / (RECT_HEIGHT + 1) + 1; i++)
        {
            //left
            this->lcd->drawFastHLine(
                0,
                leftOffsetY - i * (RECT_HEIGHT + 1),
                controlRect.width,
                this->backColor);

            //right
            this->lcd->drawFastHLine(
                0,
                rightOffsetY + i * (RECT_HEIGHT + 1) - 1,
                controlRect.width,
                this->backColor);
        }

        for (int i = 0; i < this->spectrumLineCount; i++)
        {
            //left
            this->lcd->drawFastHLine(
                i * (LINE_WIDTH + LINE_SPACE),
                leftOffsetY - this->maxLeftSpectrumData[i] - 1,
                LINE_WIDTH,
                this->maxColor);

            //right
            this->lcd->drawFastHLine(
                i * (LINE_WIDTH + LINE_SPACE),
                rightOffsetY + this->maxRightSpectrumData[i],
                LINE_WIDTH,
                this->maxColor);
        }
    }

    const int VisualizerControl::GetLineCount() const
    {
        return this->spectrumLineCount;
    }

    const int VisualizerControl::GetMaxLineLength() const
    {
        return this->spectrumMaxSize;
    }

    void VisualizerControl::Reset()
    {
        for (int i = 0; i < this->spectrumLineCount; i++)
        {
            this->nowLeftSpectrum[i] = 0;
            this->maxLeftSpectrumData[i] = 0;
            this->nowRightSpectrum[i] = 0;
            this->maxRightSpectrumData[i] = 0;
        }
    }

    void VisualizerControl::ReDraw()
    {
        ClearRect();
        DrawSpectrum(this->nowLeftSpectrum, this->nowRightSpectrum);
    }

    VisualizerControl::~VisualizerControl()
    {
        delete[] nowLeftSpectrum;
        delete[] maxLeftSpectrumData;

        delete[] nowRightSpectrum;
        delete[] maxRightSpectrumData;
    }
}