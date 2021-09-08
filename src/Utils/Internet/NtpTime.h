#pragma once

#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

namespace NtpTime
{

#define ntpServerName "2.ru.pool.ntp.org"
#define udpPort 2390
#define NTP_PACKET_SIZE 48

    typedef void (*NonBlockDelay)(unsigned long);

    unsigned long Ask_NTP_Time(NonBlockDelay nonBlockDelay, bool &isOk);
}