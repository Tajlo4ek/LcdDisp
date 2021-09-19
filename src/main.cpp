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

#define SPECTRUM_OFF_TIME 3000

/* #region func prototypes */
void NotBlockDelay(unsigned long delayTime);

void OnScreenWorkEnd();
void SetActiveScreen(BaseScreen::Screen *screen, Mode nextMode);

void CheckCommand(const String &data);

void InitWiFi();
/* #endregion */

TFT_eSPI lcd = TFT_eSPI();
#define LCD_WIDTH 160
#define LCD_HEIGHT 128
#define LCD_ROTATE 3

bool isSTA;
String serialData;

Mode nowMode;
BaseScreen::Screen *activeScreen;

BaseScreen::Screen *mainScreen;
BaseScreen::Screen *visualizerScreen;

void setup()
{
  Serial.begin(115200);
  FileSystem::Init();
  lcd.init();

  lcd.setRotation(LCD_ROTATE);
  lcd.fillScreen(TFT_BLACK);

  InitWiFi();
  HttpServer::Init(CheckCommand);

  activeScreen = nullptr;
  mainScreen = new MainScreen::MainScreen(lcd, LCD_WIDTH, LCD_HEIGHT, OnScreenWorkEnd, NotBlockDelay);
  mainScreen->SetEthernetAvailable(isSTA);

  visualizerScreen = new VisualizerScreen::VisualizerScreen(lcd, LCD_WIDTH, LCD_HEIGHT, OnScreenWorkEnd, SPECTRUM_OFF_TIME);

  SetActiveScreen(mainScreen, MAIN_MODE);
}

void InitWiFi()
{
  auto wifiConfig = WifiUtils::LoadWiFiConfig();

  lcd.drawString(F("Try connect: "), 0, 10, 1, lcd.color565(0, 255, 0));

  lcd.drawString(wifiConfig.ssid, 0, 20, 1, lcd.color565(0, 255, 0));

  lcd.drawString(F("Attempts: "), 0, 35, 1, lcd.color565(0, 255, 0));

  WifiUtils::TryConnectCallback callback = [](int tryCount)
  {
    lcd.fillRect(55, 35, 30, 8, TFT_BLACK);
    lcd.drawString(String(tryCount), 55, 35, 1, lcd.color565(0, 255, 0));
  };

  isSTA = true;
  if (WifiUtils::ConnectWifi(wifiConfig.ssid, wifiConfig.password, 20, callback) == false)
  {
    lcd.fillScreen(TFT_BLACK);
    lcd.drawString(F("can't connect. start ap"), 0, 0, 1, lcd.color565(0, 255, 0));
    lcd.drawString(F("HOME 1234567890"), 0, 15, 1, lcd.color565(0, 255, 0));
    isSTA = false;
    delay(2000);
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

void CheckCommand(const String &data)
{
  if (data[data.length() - 1] != COMMAND_STOP_CHAR)
  {
    return;
  }

  if (data.startsWith(COMMAND_SET_MODE_SPECTRUM))
  {
    SetActiveScreen(visualizerScreen, Mode::SPECTRUM_MODE);
  }
  else if (data.startsWith(COMMAND_SEND_SPECTRUM_DATA))
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
    serialData += ch;

    if (ch == COMMAND_STOP_CHAR)
    {
      CheckCommand(serialData);
      serialData.clear();
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
