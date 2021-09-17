#pragma once

#include "Arduino.h"

namespace HttpServer
{
    void Init();

    void HandleServer();

    void AddWebLog(const String &data);
}