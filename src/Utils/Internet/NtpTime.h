#pragma once

#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

namespace NtpTime
{
#define NTP_SERVER_NAME F("2.ru.pool.ntp.org")
#define UDP_PORT 2390
#define NTP_PACKET_SIZE 48

    unsigned long Ask_NTP_Time(bool &isOk);
}