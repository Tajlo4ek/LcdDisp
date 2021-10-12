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

#include "Utils/Button.h"

/* #region func prototypes */

void SetActiveScreen(int screenNum);

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

Screens::Screen *activeScreen;

Screens::Screen *mainScreen;
Screens::Screen *visualizerScreen;

std::vector<Screens::Screen *> screens;
int nowScreenNum;

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
  mainScreen = new Screens::MainScreen(&lcd);
  mainScreen->SetEthernetAvailable(isSTA);

  visualizerScreen = new Screens::VisualizerScreen(&lcd);

  screens.push_back(mainScreen);
  screens.push_back(visualizerScreen);
  SetActiveScreen(0);
}

void InitWiFi()
{
  auto wifiConfig = WifiUtils::LoadWiFiConfig();

  lcd.setTextColor(lcd.color565(0, 255, 0));
  lcd.setTextSize(1);
  lcd.drawString(F("Try connect: "), 0, 10);

  lcd.drawString(wifiConfig.ssid, 0, 20);

  lcd.drawString(F("Attempts: "), 0, 35);

  WifiUtils::TryConnectCallback callback = [](int tryCount)
  {
    lcd.fillRect(55, 35, 30, 8, TFT_BLACK);
    lcd.drawString(String(tryCount), 55, 35);
  };

  isSTA = true;
  if (WifiUtils::ConnectWifi(wifiConfig.ssid, wifiConfig.password, 20, callback) == false)
  {
    lcd.fillScreen(TFT_BLACK);
    lcd.drawString(F("can't connect. start ap"), 0, 0);

    String ssid = BASE_SSID;
    String pass = BASE_PASS;

    lcd.drawString(ssid + ' ' + pass, 0, 15);
    isSTA = false;
    delay(2000);
    WifiUtils::StartAP(ssid, pass);
  }
}

void SetActiveScreen(int screenNum)
{
  if (screenNum >= (int)screens.size())
  {
    screenNum -= screens.size();
  }
  else if (screenNum < 0)
  {
    screenNum += screens.size();
  }
  nowScreenNum = screenNum;

  for (auto screen : screens)
  {
    screen->SetVisible(false);
  }

  activeScreen = screens[nowScreenNum];
  activeScreen->SetVisible(true);
}

void CheckCommand(const String &data)
{
  if (data[data.length() - 1] != COMMAND_STOP_CHAR)
  {
    return;
  }

  if (data.startsWith(COMMAND_SET_MODE_SPECTRUM))
  {
    Serial.print(visualizerScreen->ParseMessage(data));
  }
  else if (data.startsWith(COMMAND_SEND_SPECTRUM_DATA))
  {
    visualizerScreen->ParseMessage(data);
  }
  else if (data.startsWith(COMMAND_RELOAD_SCREEN))
  {
    for (auto screen : screens)
    {
      screen->ReloadConfig();
    }
    activeScreen->ReDraw();
  }
}

/* #region parse pc data */
/*
void ParsePcData(const String &json)
{
  //String json = "{\"cpuCount\":\"1\",\"hddCount\":\"2\",\"gpuCount\":\"1\",\"ramCount\":\"1\",\"cpu\":[{\"name\":\"Intel Core i3-4160\",\"coreCount\":\"2\",\"cores\":[{\"temp\":\"44\",\"load\":\"26\",\"clock\":\"1497\",\"num\":\"1\"},{\"temp\":\"45\",\"load\":\"24\",\"clock\":\"1497\",\"num\":\"2\"}]}],\"hdd\":[{\"name\":\"Samsung SSD 860 EVO 250GB\",\"temp\":\"38\",\"used\":\"55.9\",\"written\":\"13746\"},{\"name\":\"ST1000DM010-2EP102\",\"temp\":\"34\",\"used\":\"23.4\",\"written\":\"-1\"}],\"gpu\":[{\"name\":\"NVIDIA GeForce GTX 1050 Ti\",\"temp\":\"39\",\"clock\":\"607.5\",\"loadMem\":\"9.9\",\"fanRpm\":\"0\",\"fanPr\":\"0\",\"totalMem\":\"4096\"}],\"ram\":[{\"name\":\"Generic Memory\",\"usedPr\":\"42.76\",\"total\":\"15.9\"}]}";

  int cpuCount = JsonParser::GetJsonData(json, "cpuCount").toInt();
  String cpuJson = JsonParser::GetJsonData(json, "cpu");
  ParseCpuData(cpuJson, cpuCount);

  int hddCount = JsonParser::GetJsonData(json, "hddCount").toInt();
  String hddJson = JsonParser::GetJsonData(json, "hdd");

  int gpuCount = JsonParser::GetJsonData(json, "gpuCount").toInt();
  String gpuJson = JsonParser::GetJsonData(json, "gpu");

  int ramCount = JsonParser::GetJsonData(json, "ramCount").toInt();
  String ramJson = JsonParser::GetJsonData(json, "ram");
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
      cpuScreens[i] = new HardwareScreens::CpuScreen(&lcd);
    }

    if (cpuScreenCount != 0)
    {
      //SetActiveScreen(cpuScreens[0]);
    }
  }

  for (int i = 0; i < cpuCount; i++)
  {
    cpuScreens[i]->ParseMessage(cpuDatas[i]);
  }

  delete[] cpuDatas;
}
*/

/* #endregion */

void CheckButtons()
{
  //left
  if (isBtnClicked[0] == true)
  {
    if (activeScreen->OnBtnLeftClick() == false)
    {
      SetActiveScreen(nowScreenNum + 1);
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
      SetActiveScreen(nowScreenNum - 1);
    }
    isBtnClicked[2] = false;
  }
}

void loop()
{
  //String json = "{\"cpuCount\":\"1\",\"hddCount\":\"2\",\"gpuCount\":\"1\",\"ramCount\":\"1\",\"cpu\":[{\"name\":\"Intel Core i3-4160\",\"coreCount\":\"2\",\"cores\":[{\"temp\":\"44\",\"load\":\"26\",\"clock\":\"1497\",\"num\":\"1\"},{\"temp\":\"45\",\"load\":\"24\",\"clock\":\"1497\",\"num\":\"2\"}]}],\"hdd\":[{\"name\":\"Samsung SSD 860 EVO 250GB\",\"temp\":\"38\",\"used\":\"55.9\",\"written\":\"13746\"},{\"name\":\"ST1000DM010-2EP102\",\"temp\":\"34\",\"used\":\"23.4\",\"written\":\"-1\"}],\"gpu\":[{\"name\":\"NVIDIA GeForce GTX 1050 Ti\",\"temp\":\"39\",\"clock\":\"607.5\",\"loadMem\":\"9.9\",\"fanRpm\":\"0\",\"fanPr\":\"0\",\"totalMem\":\"4096\"}],\"ram\":[{\"name\":\"Generic Memory\",\"usedPr\":\"42.76\",\"total\":\"15.9\"}]}";
  //ParsePcData(json);

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
