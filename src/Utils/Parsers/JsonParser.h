#pragma once

#include "Arduino.h"

namespace JsonParser
{
    String GetJsonData(const String &json, const String &name, bool &isOk);
    String GetJsonData(const String &json, const String &name);

    void ParseJsonArray(const String &json, const int itemCount, String* output);

    String BuildJson(const String *names, const String *data, int dataCount);
}