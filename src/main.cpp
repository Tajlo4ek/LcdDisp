#include <TFT_eSPI.h>
#include <SPI.h>

#include "Commands.h"

#include "Screens/BaseScreen.h"
#include "Screens/MainScreen.h"
#include "Screens/VisualizerScreen.h"
#include "Utils/Internet/WifiUtils.h"
#include "Utils/HTTP/HttpServer.h"
#include "Utils/FileSystem/FileSystem.h"

#include "Utils/Parsers/JsonParser.h"
#include "Screens/HardwareScreens/CpuScreen.h"

#include "Utils/Button.h"

enum Mode
{
  MAIN_MODE,
  SPECTRUM_MODE,
  VIEW_HARDWARE_MODE,
};

#define SPECTRUM_OFF_TIME 3000

/* #region func prototypes */
void NotBlockDelay(unsigned long delayTime);

void OnScreenWorkEnd();
void SetActiveScreen(BaseScreen::Screen *screen, Mode nextMode);

void CheckCommand(const String &data);

void ParsePcData(const String &json);
void ParseCpuData(const String &json, int cpuCount);

void CheckButtons();

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

#define HARDWARE_SCREEN_NOT_INIT_COUNT -1

int cpuScreenCount = HARDWARE_SCREEN_NOT_INIT_COUNT;
HardwareScreens::CpuScreen **cpuScreens;

bool isBtnClicked[3];

Utils::Button butLeft(PIN_D8, []()
                      { isBtnClicked[0] = true; });

Utils::Button btnOk(PIN_D2, []()
                    { isBtnClicked[1] = true; });

Utils::Button btnRight(PIN_D1, []()
                       { isBtnClicked[2] = true; });

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
  mainScreen = new MainScreen::MainScreen(&lcd, LCD_WIDTH, LCD_HEIGHT, OnScreenWorkEnd, NotBlockDelay);
  mainScreen->SetEthernetAvailable(isSTA);

  visualizerScreen = new VisualizerScreen::VisualizerScreen(&lcd, LCD_WIDTH, LCD_HEIGHT, OnScreenWorkEnd, SPECTRUM_OFF_TIME);

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
  else if (data.startsWith(COMMAND_RELOAD_SCREEN))
  {
    activeScreen->ReloadConfig();
  }

  Serial.print(activeScreen->ParseMessage(data));
}

void ParsePcData(const String &json)
{
  //String json = "{\"cpuCount\":\"1\",\"hddCount\":\"2\",\"gpuCount\":\"1\",\"ramCount\":\"1\",\"cpu\":[{\"name\":\"Intel Core i3-4160\",\"coreCount\":\"2\",\"cores\":[{\"temp\":\"44\",\"load\":\"26\",\"clock\":\"1497\",\"num\":\"1\"},{\"temp\":\"45\",\"load\":\"24\",\"clock\":\"1497\",\"num\":\"2\"}]}],\"hdd\":[{\"name\":\"Samsung SSD 860 EVO 250GB\",\"temp\":\"38\",\"used\":\"55.9\",\"written\":\"13746\"},{\"name\":\"ST1000DM010-2EP102\",\"temp\":\"34\",\"used\":\"23.4\",\"written\":\"-1\"}],\"gpu\":[{\"name\":\"NVIDIA GeForce GTX 1050 Ti\",\"temp\":\"39\",\"clock\":\"607.5\",\"loadMem\":\"9.9\",\"fanRpm\":\"0\",\"fanPr\":\"0\",\"totalMem\":\"4096\"}],\"ram\":[{\"name\":\"Generic Memory\",\"usedPr\":\"42.76\",\"total\":\"15.9\"}]}";

  int cpuCount = JsonParser::GetJsonData(json, "cpuCount").toInt();
  String cpuJson = JsonParser::GetJsonData(json, "cpu");
  ParseCpuData(cpuJson, cpuCount);

  /*int hddCount = JsonParser::GetJsonData(json, "hddCount").toInt();
  String hddJson = JsonParser::GetJsonData(json, "hdd");

  int gpuCount = JsonParser::GetJsonData(json, "gpuCount").toInt();
  String gpuJson = JsonParser::GetJsonData(json, "gpu");

  int ramCount = JsonParser::GetJsonData(json, "ramCount").toInt();
  String ramJson = JsonParser::GetJsonData(json, "ram");*/
}

void ParseCpuData(const String &json, int cpuCount)
{
  String *cpuDatas = new String[cpuCount];
  JsonParser::ParseJsonArray(json, cpuCount, cpuDatas);

  if (cpuScreenCount == HARDWARE_SCREEN_NOT_INIT_COUNT || cpuScreenCount != cpuCount)
  {
    delete[] cpuScreens;

    cpuScreens = new HardwareScreens::CpuScreen *[cpuCount];
    cpuScreenCount = cpuCount;

    for (int i = 0; i < cpuCount; i++)
    {
      cpuScreens[i] = new HardwareScreens::CpuScreen(&lcd, OnScreenWorkEnd);
    }

    if (cpuScreenCount != 0)
    {
      SetActiveScreen(cpuScreens[0], VIEW_HARDWARE_MODE);
    }
  }

  for (int i = 0; i < cpuCount; i++)
  {
    cpuScreens[i]->ParseMessage(cpuDatas[i]);
  }

  delete[] cpuDatas;
}

void CheckButtons()
{
  //left
  if (isBtnClicked[0] == true)
  {
    if (activeScreen->OnBtnLeftClick() == false)
    {
      Serial.println("left");
    }
    isBtnClicked[0] = false;
  }

  //center
  if (isBtnClicked[1] == true)
  {
    activeScreen->OnBtnCenterClick();
    isBtnClicked[1] = false;
    Serial.println("center");
  }

  //right
  if (isBtnClicked[2] == true)
  {
    if (activeScreen->OnBtnRightClick() == false)
    {
      Serial.println("right");
    }
    isBtnClicked[2] = false;
  }
}

/* #region Loop */

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
  CheckButtons();
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
  //String json = "{\"cpuCount\":\"1\",\"hddCount\":\"2\",\"gpuCount\":\"1\",\"ramCount\":\"1\",\"cpu\":[{\"name\":\"Intel Core i3-4160\",\"coreCount\":\"2\",\"cores\":[{\"temp\":\"44\",\"load\":\"26\",\"clock\":\"1497\",\"num\":\"1\"},{\"temp\":\"45\",\"load\":\"24\",\"clock\":\"1497\",\"num\":\"2\"}]}],\"hdd\":[{\"name\":\"Samsung SSD 860 EVO 250GB\",\"temp\":\"38\",\"used\":\"55.9\",\"written\":\"13746\"},{\"name\":\"ST1000DM010-2EP102\",\"temp\":\"34\",\"used\":\"23.4\",\"written\":\"-1\"}],\"gpu\":[{\"name\":\"NVIDIA GeForce GTX 1050 Ti\",\"temp\":\"39\",\"clock\":\"607.5\",\"loadMem\":\"9.9\",\"fanRpm\":\"0\",\"fanPr\":\"0\",\"totalMem\":\"4096\"}],\"ram\":[{\"name\":\"Generic Memory\",\"usedPr\":\"42.76\",\"total\":\"15.9\"}]}";
  //ParsePcData(json);

  MyLoop();
}

/* #endregion */
