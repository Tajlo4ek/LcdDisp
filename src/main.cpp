#include <TFT_eSPI.h>
#include <SPI.h>

#include "Commands.h"

#include "Screens/BaseScreen.h"
#include "Screens/MainScreen.h"
#include "Utils/Internet/WifiUtils.h"
#include "Utils/HTTP/HttpServer.h"
#include "Utils/FileSystem/FileSystem.h"

enum Mode
{
  CLOCK_MODE,
  //SPECTRUM_MODE,
};

/* #region  func prototypes */
void NotBlockDelay(unsigned long delayTime);

//void SpectrumCheck();
//String GetSpectrumData();

void SetActiveScreen(BaseScreen::Screen *screen);

//void CheckCommand(String &data);
//void ParseSpectrum(String &data);

void InitWiFi();
/* #endregion */

TFT_eSPI lcd = TFT_eSPI();
#define lcdWidth 160
#define lcdHeight 128
#define lcdRotate 3

bool isSTA;

//#define SpectrumOffTime 3000
//SpectrumDrawer::SpectrumDrawer *spectrumDrawer;
//MillisTimer::Timer spectrumCheckTimer(SpectrumCheck, SpectrumOffTime); //need of spectrum SpectrumOffTime ms

Mode nowMode;
String serialData = "";

BaseScreen::Screen *activeScreen;
BaseScreen::Screen *mainScreen;

void setup()
{
  FileSystem::Init();
  lcd.init();
  Serial.begin(115200);

  lcd.setRotation(lcdRotate);
  lcd.fillScreen(TFT_BLACK);

  InitWiFi();
  HttpServer::Init();

  //spectrumDrawer = new SpectrumDrawer::SpectrumDrawer(lcd, lcdWidth, lcdHeight);

  activeScreen = nullptr;
  mainScreen = new MainScreen::MainScreen(lcd, lcdWidth, lcdHeight, NotBlockDelay);
  mainScreen->SetEthernetAvailable(isSTA);

  SetActiveScreen(mainScreen);
}

void InitWiFi()
{
  auto wifiConfig = WifiUtils::LoadWiFiConfig();

  auto text = String(F("Try connect: "));
  lcd.drawString(text, 0, 10, 1, lcd.color565(0, 255, 0));

  text = wifiConfig.ssid;
  lcd.drawString(text, 0, 20, 1, lcd.color565(0, 255, 0));

  text = String(F("Attempts: "));
  lcd.drawString(text, 0, 35, 1, lcd.color565(0, 255, 0));

  WifiUtils::TryConnectCallback callback = [](int tryCount)
  {
    auto text = String("") + tryCount;
    lcd.fillRect(55, 35, 30, 8, TFT_BLACK);
    lcd.drawString(text, 55, 35, 1, lcd.color565(0, 255, 0));
  };

  isSTA = true;
  if (!WifiUtils::ConnectWifi(wifiConfig.ssid, wifiConfig.password, 20, callback))
  {
    lcd.fillScreen(TFT_BLACK);
    lcd.drawString(String(F("can't connect. start ap")), 0, 0, 1, lcd.color565(0, 255, 0));
    lcd.drawString(String(F("HOME 1234567890")), 0, 15, 1, lcd.color565(0, 255, 0));
    isSTA = false;
    delay(500);
    WifiUtils::StartAP(F("HOME"), F("1234567890"));
  }
}

void SetActiveScreen(BaseScreen::Screen *screen)
{
  if (activeScreen != nullptr)
  {
    activeScreen->LeaveFocus();
  }
  activeScreen = screen;
  activeScreen->EnterFocus();
}

/*void SetModeSpectrum()
{
  //ResetMode();

  //nowMode = SPECTRUM_MODE;
  //spectrumDrawer->Reset();
  //spectrumCheckTimer.Start();
}*/

/*
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

*/

/* #region Loop */

void MyLoop()
{
  while (Serial.available())
  {
    char ch = (char)Serial.read();

    if (ch == STOP_CHAR)
    {
      //CheckCommand(serialData);
      serialData = "";
    }
    else
    {
      serialData += ch;
    }
  }

  activeScreen->Loop();
  HttpServer::HandleServer();
}

void NotBlockDelay(unsigned long delayTime)
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