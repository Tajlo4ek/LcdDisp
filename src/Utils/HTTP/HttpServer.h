#pragma once

#include "Arduino.h"

namespace HttpServer
{
    typedef std::function<void(const String &)> GetCommandCallback;

    void Init(GetCommandCallback callback);

    void HandleServer();

    void AddWebLog(const String &data);
}