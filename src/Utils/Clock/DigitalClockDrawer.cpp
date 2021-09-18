#include "DigitalClockDrawer.h"

#include "FileNames.h"
#include "Utils/WeatherImages.h"

#include "Utils/FileSystem/FileSystem.h"
#include "Utils/DrawUtils/Color.h"
#include "Utils/Parsers/JsonParser.h"

namespace ClockDrawer
{
#define SPACE_SIZE 3
#define CONFIG_BACK_COLOR F("backColor")
#define CONFIG_CLOCK_MAIN_COLOR F("clockMainColor")
#define CONFIG_CLOCK_SECOND_COLOR F("clockSecondColor")

    DigitalClockDrawer::DigitalClockDrawer(TFT_eSPI &lcd, int width, int height, Clock::Clock &clock)
        : BaseClockDrawer::BaseClockDrawer(lcd, width, height, clock)
    {

        int numSizeX = (width - SPACE_SIZE * 4) / 5;
        int numSizeY = numSizeX * 2;
        while (numSizeY % 7 != 0)
        {
            numSizeY++;
        }

        this->blockWidth = numSizeX;
        this->blockHeight = numSizeY / 7;
        if (blockHeight % 2 != 1)
        {
            blockHeight--;
        }

        this->spaceDiv4 = (this->lcdHeight - this->blockWidth * 2) / 4;
    }

    void DigitalClockDrawer::ReloadConfig()
    {
        auto json = FileSystem::ReadFile(DIGITAL_CLOCK_CONFIG_PATH);
        if (json.isEmpty())
        {
            this->CreateDefaultConfig();
        }

        const uint colorCount = 3;
        String colorNames[colorCount]{
            CONFIG_BACK_COLOR,
            CONFIG_CLOCK_MAIN_COLOR,
            CONFIG_CLOCK_SECOND_COLOR,
        };

        uint16_t *colors[colorCount]{
            &this->backColor,
            &this->clockMainColor,
            &this->clockSecondColor,
        };

        bool loadRes = DrawUtils::LoadColorsFromJson(json, colorNames, colors, colorCount);

        if (loadRes == false)
        {
            this->CreateDefaultConfig();
            this->ReloadConfig();
        }
    }

    void DigitalClockDrawer::CreateDefaultConfig()
    {
        const uint configCount = 3;
        String configNames[configCount]{
            CONFIG_BACK_COLOR,
            CONFIG_CLOCK_MAIN_COLOR,
            CONFIG_CLOCK_SECOND_COLOR,
        };

        String datas[configCount]{
            DrawUtils::GetJsonColor(0, 0, 0),
            DrawUtils::GetJsonColor(0, 0, 255),
            DrawUtils::GetJsonColor(0, 0, 200),
        };

        FileSystem::WriteFile(
            DIGITAL_CLOCK_CONFIG_PATH,
            JsonParser::BuildJson(configNames, datas, configCount));
    }

    void DigitalClockDrawer::Init()
    {
        this->ReloadConfig();
        this->lcd->fillScreen(this->backColor);
    }

    void DigitalClockDrawer::WeatherChanged()
    {

        auto weatherNow = this->weatherData->GetCurrentValue();

        auto weatherImage = WeatherImages::GetImage(weatherNow.imageName);

        uint offsetX = SPACE_SIZE;
        uint offsetY = this->spaceDiv4 * 2 + this->blockWidth * 2;

        this->lcd->fillRect(0, offsetY, this->lcdWidth, this->lcdHeight - offsetY, this->backColor);

        uint x = 0;
        uint y = 0;
        for (uint i = 0; i < WeatherImages::ImageByteCount; i++)
        {
            auto item = weatherImage[i];
            for (uint j = 0; j < sizeof(item) * 8; j++)
            {
                if (x >= WeatherImages::ImageSize)
                {
                    y++;
                    x = 0;
                }

                this->lcd->drawPixel(x + offsetX, y + offsetY, (item % 2) ? this->clockMainColor : this->backColor);

                item /= 2;
                x++;
            }
        }

        const uint maxStringSize = 12;
        auto text = weatherNow.description;

        String words[6];
        int nowInd = 0;
        String buf = String();
        for (uint i = 0; i < text.length(); i++)
        {
            auto ch = text[i];

            if (ch == ' ')
            {
                words[nowInd] = buf;
                buf = String();
                nowInd++;
            }
            else
            {
                buf += ch;
            }
        }

        if (buf.length() != 0)
        {
            words[nowInd] = buf;
            nowInd++;
        }

        buf = words[0] + F(" ");
        for (int i = 1; i < nowInd; i++)
        {
            if (buf.length() + words[i].length() >= maxStringSize)
            {
                this->DrawString(buf, WeatherImages::ImageSize + offsetX * 2, offsetY, this->clockMainColor, 1);
                offsetY += 8;
                buf = String();
            }

            buf += words[i] + F(" ");
        }

        if (buf.length() != 0)
        {
            this->DrawString(buf, WeatherImages::ImageSize + offsetX * 2, offsetY, this->clockMainColor, 1);
        }

        this->lcd->setTextSize(2);
        auto textWidth = this->lcd->textWidth(weatherNow.temp);
        this->DrawString(weatherNow.temp, this->lcdWidth - textWidth, this->spaceDiv4 * 5 / 2 + this->blockWidth * 2, this->clockMainColor, 2);
    }

    void DigitalClockDrawer::MessageChanged()
    {
        auto text = this->message->GetPrevievValue();
        this->DrawCentralText(text, 2, this->backColor, 1);

        text = this->message->GetCurrentValue();
        this->DrawCentralText(text, 2, this->clockMainColor, 1);
    }

    void DigitalClockDrawer::TimeChanged()
    {
        auto state = this->myClock->GetTime();
        DrawNum((int)state.hour / 10, SPACE_SIZE, this->spaceDiv4);
        DrawNum((int)state.hour % 10, SPACE_SIZE * 2 + this->blockWidth, this->spaceDiv4);

        DrawNum((int)state.minute / 10, this->lcdWidth - (SPACE_SIZE + this->blockWidth) * 2, this->spaceDiv4);
        DrawNum((int)state.minute % 10, this->lcdWidth - SPACE_SIZE - this->blockWidth, this->spaceDiv4);

        int delta = (this->lcdWidth - (SPACE_SIZE + this->blockWidth) * 2 - (SPACE_SIZE + this->blockWidth) * 2);
        int dotRadius = delta * 30 / 100 / 2;

        int dotX = this->lcdWidth / 2;
        int dotY = (this->blockWidth - dotRadius) / 2;

        uint16_t dotColor = (int)state.second % 2 == 1 ? this->clockMainColor : this->backColor;
        this->lcd->fillEllipse(dotX, this->spaceDiv4 + dotY, dotRadius, dotRadius, dotColor);
        this->lcd->fillEllipse(dotX, this->spaceDiv4 + dotY + this->blockWidth - this->blockHeight, dotRadius, dotRadius, dotColor);
    }

    void DigitalClockDrawer::DateChanged()
    {
        int y = this->spaceDiv4 + this->blockWidth * 2;
        this->lcd->fillRect(0, y, this->lcdWidth, 16, this->backColor);

        auto text = this->myClock->GetDateString();
        this->DrawCentralText(text, y, this->clockMainColor, 2);
    }

    void DigitalClockDrawer::TimeSyncChanged()
    {
        auto textY = this->spaceDiv4 + this->blockWidth * 1.5f;

        DrawCentralText(String(F("not")), textY - 7, this->backColor, 1);

        DrawCentralText(String(F("sync")), textY + 1, this->backColor, 1);

        if (this->isTimeSync->GetCurrentValue() == false)
        {
            DrawCentralText(String(F("not")), textY - 7, TFT_RED, 1);

            DrawCentralText(String(F("sync")), textY + 1, TFT_RED, 1);
        }
    }

    void DigitalClockDrawer::DrawCentralText(const String &text, int y, uint16_t color, int textSize) const
    {
        this->lcd->setTextSize(textSize);
        int textWidth = this->lcd->textWidth(text);
        this->lcd->fillRect((this->lcdWidth - textWidth) / 2, y, textWidth, 8 * textSize, this->backColor);
        this->DrawString(text, (this->lcdWidth - textWidth) / 2, y, color, textSize);
    }

    void DigitalClockDrawer::DrawString(const String &text, int x, int y, uint16_t color, int textSize) const
    {
        this->lcd->setTextSize(textSize);
        this->lcd->setTextColor(color);
        this->lcd->drawString(text, x, y);
    }

    void DigitalClockDrawer::DrawNum(int num, int x, int y) const
    {
        if (num > 9)
        {
            return;
        }

        //top
        if (num != 1 && num != 4)
        {
            DrawHorBlock(x, y, this->clockMainColor);
        }
        else
        {
            DrawHorBlock(x, y, this->backColor);
        }

        //left top
        if (num != 1 && num != 2 && num != 3 && num != 7)
        {
            DrawVerBlock(x, y, this->clockMainColor);
        }
        else
        {
            DrawVerBlock(x, y, this->backColor);
        }

        //right top
        if (num != 5 && num != 6)
        {
            DrawVerBlock(x + this->blockWidth - this->blockHeight + 1, y, this->clockMainColor);
        }
        else
        {
            DrawVerBlock(x + this->blockWidth - this->blockHeight + 1, y, this->backColor);
        }

        //center
        if (num > 1 && num != 7)
        {
            DrawHorBlock(x, y + this->blockWidth - this->blockHeight + 1, this->clockMainColor);
        }
        else
        {
            DrawHorBlock(x, y + this->blockWidth - this->blockHeight + 1, this->backColor);
        }

        //left bottom
        if (num == 0 || num == 2 || num == 6 || num == 8)
        {
            DrawVerBlock(x, y + this->blockWidth - this->blockHeight + 1, this->clockMainColor);
        }
        else
        {
            DrawVerBlock(x, y + this->blockWidth - this->blockHeight + 1, this->backColor);
        }

        //right bottom
        if (num != 2)
        {
            DrawVerBlock(x + this->blockWidth - this->blockHeight + 1, y + this->blockWidth - this->blockHeight + 1, this->clockMainColor);
        }
        else
        {
            DrawVerBlock(x + this->blockWidth - this->blockHeight + 1, y + this->blockWidth - this->blockHeight + 1, this->backColor);
        }

        //bottom
        if (num != 1 && num != 4 && num != 7)
        {
            DrawHorBlock(x, y + (this->blockWidth - this->blockHeight + 1) * 2, this->clockMainColor);
        }
        else
        {
            DrawHorBlock(x, y + (this->blockWidth - this->blockHeight + 1) * 2, this->backColor);
        }
    }

    void DigitalClockDrawer::DrawVerBlock(int x, int y, uint16_t color) const
    {
        int blockHeightDiv2 = this->blockHeight / 2;

        this->DrawSpecLine(x + blockHeightDiv2,
                           y + 1 + blockHeightDiv2,
                           x + blockHeightDiv2,
                           y + blockWidth - 1 - blockHeightDiv2,
                           color,
                           this->clockSecondColor);

        for (int i = 1; i < blockHeightDiv2; i++)
        {
            this->DrawSpecLine(x + blockHeightDiv2 + i,
                               y + 1 + i + blockHeightDiv2,
                               x + blockHeightDiv2 + i,
                               y + blockWidth - 1 - i - blockHeightDiv2,
                               i == blockHeightDiv2 - 1 ? this->clockSecondColor : color,
                               this->clockSecondColor);

            this->DrawSpecLine(x + blockHeightDiv2 - i,
                               y + 1 + i + blockHeightDiv2,
                               x + blockHeightDiv2 - i,
                               y + blockWidth - 1 - i - blockHeightDiv2,
                               i == blockHeightDiv2 - 1 ? this->clockSecondColor : color,
                               this->clockSecondColor);
        }
    }

    void DigitalClockDrawer::DrawHorBlock(int x, int y, uint16_t color) const
    {
        int blockHeightDiv2 = this->blockHeight / 2;

        this->DrawSpecLine(x + 1 + blockHeightDiv2,
                           y + blockHeightDiv2,
                           x + blockWidth - 1 - blockHeightDiv2,
                           y + blockHeightDiv2,
                           color,
                           this->clockSecondColor);

        for (int i = 1; i < blockHeightDiv2; i++)
        {
            this->DrawSpecLine(x + 1 + i + blockHeightDiv2,
                               y + blockHeightDiv2 + i,
                               x + blockWidth - 1 - i - blockHeightDiv2,
                               y + blockHeightDiv2 + i,
                               i == blockHeightDiv2 - 1 ? this->clockSecondColor : color,
                               this->clockSecondColor);

            this->DrawSpecLine(x + 1 + i + blockHeightDiv2,
                               y + blockHeightDiv2 - i,
                               x + blockWidth - 1 - i - blockHeightDiv2,
                               y + blockHeightDiv2 - i,
                               i == blockHeightDiv2 - 1 ? this->clockSecondColor : color,
                               this->clockSecondColor);
        }
    }

    void DigitalClockDrawer::DrawSpecLine(int x0, int y0, int x1, int y1, uint16_t mainColor, uint16_t secondColor) const
    {
        this->lcd->drawLine(x0, y0, x1, y1, mainColor);
        if (mainColor != secondColor)
        {
            this->lcd->drawPixel(x0, y0, secondColor);
            this->lcd->drawPixel(x1, y1, secondColor);
        }
    }

}