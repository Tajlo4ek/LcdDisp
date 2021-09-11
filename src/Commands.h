#pragma once

#include "Arduino.h"

namespace Commands
{
    const char stopChar = '|';
    const char splitChar = ';';
    const String setModeSpectrum = "startSpectrum";
    const String setLineCount = "line";
    const String sendSpectrumData = "sendSpectrum";
    const String setMaxData = "maxData";
    const String logToken = "[LOG] ";
}