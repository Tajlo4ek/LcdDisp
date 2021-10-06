#include "Button.h"

#include <FunctionalInterrupt.h>

namespace Utils
{

#define CLICK_DELAY 150

    Button::Button(int pin, ButtonClickCallback callback)
    {
        this->clickCallback = callback;
        this->pin = pin;

        pinMode(pin, INPUT);
        attachInterrupt(pin, std::bind(&Button::Check, this), RISING);

        lastClick = 0;
    }

    void Button::Check()
    {
        unsigned long time = millis();

        bool isClick = (time - lastClick) > CLICK_DELAY;
        this->lastClick = time;
        if (isClick)
        {
            this->clickCallback();
        }
    }

    Button::~Button()
    {
        detachInterrupt(pin);
        clickCallback = nullptr;
    }
}