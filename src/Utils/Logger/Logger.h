#pragma once

#include "Arduino.h"

namespace Logger
{

#define SERIAL_SPEED 115200

    void SerialLog(String data);

    void WebLog(String data);
}
