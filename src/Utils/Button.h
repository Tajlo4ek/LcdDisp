#pragma once

#include <Arduino.h>

namespace Utils
{
    class Button
    {
        typedef std::function<void()> ButtonClickCallback;

    private:
        ButtonClickCallback clickCallback;
        int pin;

        unsigned long lastClick;
        unsigned long startClick;

        void Check();

    public:
        Button(int pin, ButtonClickCallback callback);
        ~Button();
    };

}