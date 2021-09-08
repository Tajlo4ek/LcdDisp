#include <TFT_eSPI.h>
#include <SPI.h>

#include "Commands.h"

#include "Utils/Internet/WifiUtils.h"
#include "Utils/Timer/MillisTimer.h"
#include "Utils/Timer/TimerManager.h"
#include "Utils/Clock/Clock.h"
#include "Utils/Clock/DigitalClockDrawer.h"
#include "Utils/Spectrum/SpectrumDrawer.h"
#include "Utils/Internet/NtpTime.h"
#include "Utils/Internet/Weather.h"
#include "Utils/HTTP/HttpServer.h"
#include "Utils/FileSystem/FileSystem.h"

#include "Utils/Controls/Keyboard.h"

enum Mode
{
  CLOCK_MODE,
  SPECTRUM_MODE,
};

/* #region  func prototypes */
void NonBlockDelay(unsigned long delayTime);

void ClockTick();
void CheckTimeSync();
void GetWeather();

void SpectrumCheck();
String GetSpectrumData();

void SetModeSpectrum();
void SetModeClock();
void ResetMode();

void CheckCommand(String &data);
void ParseSpectrum(String &data);

void InitWiFi();
/* #endregion */

TFT_eSPI lcd = TFT_eSPI();
#define lcdWidth 160
#define lcdHeight 128
#define lcdRotate 3

bool isSTA;
bool isTimeSync;

Clock::Clock myClock;
ClockDrawer::BaseClockDrawer *clockDrawer;
MillisTimer::Timer clockTimer([]() { myClock.Tick(); }, 500);                           //clock tick 500 ms
MillisTimer::Timer timeSynchronizeTimer(CheckTimeSync, 60000);                          //check is need sync time 1 minute
MillisTimer::Timer resetSynchronizeTimer([]() { isTimeSync = false; }, 60 * 60 * 5000); //need sync time 1 hour
MillisTimer::Timer weatherTimer(GetWeather, 10 * 60 * 1000);                            //update weather 10 minute
TimerManager::TimerManager clockTimersManager;

#define SpectrumOffTime 3000
SpectrumDrawer::SpectrumDrawer *spectrumDrawer;
MillisTimer::Timer spectrumCheckTimer(SpectrumCheck, SpectrumOffTime); //need of spectrum SpectrumOffTime ms

Mode nowMode;
String serialData = "";

void setup()
{
  FileSystem::Init();
  lcd.init();
  Serial.begin(115200);

  lcd.setRotation(lcdRotate);
  lcd.fillScreen(TFT_BLACK);

  InitWiFi();
  HttpServer::Init();

  clockDrawer = new ClockDrawer::DigitalClockDrawer(lcd, lcdWidth, lcdHeight, myClock);
  clockTimersManager.AddTimer(clockTimer);
  clockTimersManager.AddTimer(timeSynchronizeTimer);
  clockTimersManager.AddTimer(resetSynchronizeTimer);
  clockTimersManager.AddTimer(weatherTimer);

  spectrumDrawer = new SpectrumDrawer::SpectrumDrawer(lcd, lcdWidth, lcdHeight);

  SetModeClock();
}

void InitWiFi()
{
  auto wifiConfig = FileSystem::GetWiFiData();

  auto text = String(F("Try connect: "));
  lcd.drawString(text, 0, 10, 1, lcd.color565(0, 255, 0));

  text = wifiConfig.ssid;
  lcd.drawString(text, 0, 20, 1, lcd.color565(0, 255, 0));

  text = String(F("Attempts: "));
  lcd.drawString(text, 0, 35, 1, lcd.color565(0, 255, 0));

  WifiUtils::TryConnectCallback callback = [](int tryCount) {
    auto text = String("") + tryCount;
    lcd.fillRect(55, 35, 30, 8, TFT_BLACK);
    lcd.drawString(text, 55, 35, 1, lcd.color565(0, 255, 0));
  };

  isSTA = true;
  if (!WifiUtils::ConnectWifi(wifiConfig.ssid, wifiConfig.password, 20, callback))
  {
    lcd.drawString(String(F("can't connect. start ap")), 0, 0, 1, lcd.color565(0, 255, 0));
    lcd.drawString(String(F("HOME 1234567890")), 0, 15, 1, lcd.color565(0, 255, 0));
    isSTA = false;
    delay(500);
    WifiUtils::StartAP(F("HOME"), F("1234567890"));
  }
}

/* #region  Set mode */

void ResetMode()
{
  lcd.fillScreen(TFT_BLACK);
  clockTimersManager.StopAll();
  spectrumCheckTimer.Stop();
}

void SetModeClock()
{
  ResetMode();

  nowMode = CLOCK_MODE;

  isTimeSync = false;
  clockDrawer->SetWeather(1000, F("weather not sync"), F("abort"));
  clockDrawer->SetMessage(String(F("IP: ")) + WifiUtils::GetIpString());
  CheckTimeSync();
  GetWeather();
  clockTimersManager.StartAll();
}

void SetModeSpectrum()
{
  ResetMode();

  nowMode = SPECTRUM_MODE;
  spectrumDrawer->Reset();
  spectrumCheckTimer.Start();
}

/* #endregion */

void CheckTimeSync()
{
  if (!isSTA)
  {
    clockDrawer->SetTimeSync(false);
    return;
  }

  if (!isTimeSync)
  {
    auto time = NtpTime::Ask_NTP_Time(NonBlockDelay, isTimeSync);
    if (isTimeSync)
    {
      myClock.ParseFromNtp(time);
    }
    clockDrawer->SetTimeSync(isTimeSync);
  }
}

void GetWeather()
{
  if (!isSTA)
  {
    return;
  }

  bool isOk = false;
  auto weather = Weather::GetWether(NonBlockDelay, isOk);
  if (isOk)
  {
    clockDrawer->SetWeather(weather.temp, weather.description, weather.imageName);
  }
}

void SpectrumCheck()
{
  if (nowMode == SPECTRUM_MODE)
  {
    if (millis() - spectrumDrawer->GetLastUpdateTime() > SpectrumOffTime)
    {
      SetModeClock();
    }
  }
}

String GetSpectrumData()
{
  String data = SET_LINE_COUNT;
  data += spectrumDrawer->GetLineCount();
  data += STOP_CHAR;
  data += SET_MAX_DATA;
  data += spectrumDrawer->GetMaxLineLength();
  data += STOP_CHAR;
  return data;
}

/* #region  check command */

void CheckCommand(String &data)
{
  if (data.startsWith(SET_MODE_SPECTRUM))
  {
    SetModeSpectrum();
    Serial.print(GetSpectrumData());
  }
  else if (data.startsWith(SEND_SPECTRUM_DATA))
  {
    if (nowMode != SPECTRUM_MODE)
    {
      return;
    }
    ParseSpectrum(data);
  }
}

void ParseSpectrum(String &data)
{
  auto spectrumLen = spectrumDrawer->GetLineCount();
  auto spectrumLeft = new byte[spectrumLen];
  auto spectrumRight = new byte[spectrumLen];
  int dateLen = data.length();

  byte next = 0;
  int spNum = 0;
  for (int pos = String(SEND_SPECTRUM_DATA).length(); pos < dateLen; pos++)
  {
    char ch = data[pos];
    if (ch != SPLIT_CHAR)
    {
      next *= 10;
      next += ch - '0';
    }
    else
    {
      if (spNum < spectrumLen)
      {
        spectrumLeft[spNum] = next;
      }
      else
      {
        spectrumRight[spNum - spectrumLen] = next;
      }
      next = 0;
      spNum++;
    }
  }
  spectrumDrawer->DrawSpectrum(spectrumLeft, spectrumRight);
  delete[] spectrumLeft;
  delete[] spectrumRight;
}

/* #endregion */

/* #region Loop */

void MyLoop()
{
  while (Serial.available())
  {
    char ch = (char)Serial.read();

    if (ch == STOP_CHAR)
    {
      CheckCommand(serialData);
      serialData = "";
    }
    else
    {
      serialData += ch;
    }
  }

  clockTimersManager.Tick();
  spectrumCheckTimer.Tick();

  HttpServer::HandleServer();
}

void NonBlockDelay(unsigned long delayTime)
{
  unsigned long start = millis();
  while (1)
  {
    MyLoop();
    if (millis() - start > delayTime)
    {
      break;
    }
  }
}

void loop()
{
  MyLoop();
}

/* #endregion */