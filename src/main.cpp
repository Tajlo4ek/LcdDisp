#include <TFT_eSPI.h>
#include <SPI.h>

#include "Commands.h"

#include "Screens/BaseScreen.h"
#include "Screens/MainScreen.h"
#include "Screens/VisualizerScreen.h"
#include "Utils/Internet/WifiUtils.h"
#include "Utils/HTTP/HttpServer.h"
#include "Utils/FileSystem/FileSystem.h"
#include "Utils/Logger/Logger.h"

enum Mode
{
  MAIN_MODE,
  SPECTRUM_MODE,
};

#define SpectrumOffTime 3000

/* #region func prototypes */
void NotBlockDelay(unsigned long delayTime);

void OnScreenWorkEnd();
void SetActiveScreen(BaseScreen::Screen *screen, Mode nextMode);

void CheckCommand(String &data);

void InitWiFi();
/* #endregion */

TFT_eSPI lcd = TFT_eSPI();
#define lcdWidth 160
#define lcdHeight 128
#define lcdRotate 3

bool isSTA;
String serialData = "";

Mode nowMode;
BaseScreen::Screen *activeScreen;

BaseScreen::Screen *mainScreen;
BaseScreen::Screen *visualizerScreen;

void setup()
{
  FileSystem::Init();
  lcd.init();

  lcd.setRotation(lcdRotate);
  lcd.fillScreen(TFT_BLACK);

  InitWiFi();
  HttpServer::Init();

  activeScreen = nullptr;
  mainScreen = new MainScreen::MainScreen(lcd, lcdWidth, lcdHeight, OnScreenWorkEnd, NotBlockDelay);
  mainScreen->SetEthernetAvailable(isSTA);

  visualizerScreen = new VisualizerScreen::VisualizerScreen(lcd, lcdWidth, lcdHeight, OnScreenWorkEnd, SpectrumOffTime);

  SetActiveScreen(mainScreen, MAIN_MODE);
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

void OnScreenWorkEnd()
{
  SetActiveScreen(mainScreen, Mode::MAIN_MODE);
}

void SetActiveScreen(BaseScreen::Screen *screen, Mode nextMode)
{
  if (activeScreen != nullptr)
  {
    activeScreen->LeaveFocus();
  }
  activeScreen = screen;
  nowMode = nextMode;
  activeScreen->EnterFocus();
}

void CheckCommand(String &data)
{
  if (data.startsWith(Commands::setModeSpectrum))
  {
    SetActiveScreen(visualizerScreen, Mode::SPECTRUM_MODE);
  }
  else if (data.startsWith(Commands::sendSpectrumData))
  {
    if (nowMode != SPECTRUM_MODE)
    {
      return;
    }
  }

  Serial.print(activeScreen->ParseMessage(data));
}

/* #region Loop */

auto asd = millis();

void MyLoop()
{
  while (Serial.available())
  {
    char ch = (char)Serial.read();

    if (ch == Commands::stopChar)
    {
      CheckCommand(serialData);
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