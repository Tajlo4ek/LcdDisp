#pragma once

#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

namespace NtpTime
{
#define ntpServerName String(F("2.ru.pool.ntp.org"))
#define udpPort 2390
#define NTP_PACKET_SIZE 48

    typedef std::function<void(unsigned int)> NotBlockDelay;

    unsigned long Ask_NTP_Time(NotBlockDelay notBlockDelay, bool &isOk);
}