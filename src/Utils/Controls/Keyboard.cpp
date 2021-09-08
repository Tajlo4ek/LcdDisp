#include <Arduino.h>
#include <TFT_eSPI.h>

namespace Keyboard
{
    void Draw(TFT_eSPI &lcd, int x, int y);

    void MoveLeft();

    void MoveRight();

    void MoveUp();

    void MoveDown();
}