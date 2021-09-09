#pragma once

#include "Arduino.h"

namespace JsonParser
{
    String GetJsonData(String &json, String name, bool &isOk);
    String GetJsonData(String &json, String name);

    String BuildJson(String *names, String *data, int dataCount);
}