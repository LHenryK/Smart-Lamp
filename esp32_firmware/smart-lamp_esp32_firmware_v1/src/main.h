/*
 * Smart Lamp - project
 *
 * Author: Linus H. Kroog
 * E-Mail: linus.henry.kroog@bbs-winsen.de
 *
 * File name: main.h
 */

#include <Arduino.h>

#include <WiFi.h>

#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <RotaryEncoder.h>

#include <Adafruit_NeoPixel.h>

//
//
//
// SECTION: --- Varables ---
//

// SECTION: -- General --
#define PIN 14
#define PixelNum 42

int rotaryForwardsPin = 2;
int rotaryBackwardsPin = 4;
int rotaryButtonPin = 27;

long altePosition = 0;

RotaryEncoder encoder(rotaryBackwardsPin, rotaryForwardsPin, RotaryEncoder::LatchMode::TWO03);

unsigned long serialBaud = 115200;

int currentRChannel = 255;
int currentGChannel = 255;
int currentBChannel = 255;

int currentBrightness = 50;

int pixelSegmentLength = 7;

bool didLedModeChanged = false;

bool isMenuOpen = false;
int menuToggleTime = 6000;
int currentMenuToggleTime = 0;

// Temporary varables for saving loaded values out of flash
String tempVarInpName = "";

String tempVarInpR = "";
String tempVarInpG = "";
String tempVarInpB = "";
String tempVarInpH = "";

String tempVarSelM = "";

String tempVarConSta = "";
String tempVarInpWID = "";
String tempVarInpWPa = "";
String tempVarInpBIP = "";
String tempVarInpBUs = "";
String tempVarInpBPa = "";

// SECTION: -- Lamp Mode Varables --
enum LampModes
{
  NOMODE,
  WHITEMODE,
  TESTMODE,
  PARTYMODE,
  PARINGMODE,
  LIVEMODE,
};

LampModes currentLampMode = NOMODE;

// Rotary Encoder States
bool isRotaryForwardsHigh = false;
bool isRotaryBackwardsHigh = false;

bool isRotaryButtonHigh = false;

//
// SECTION: -- Network Varables --
bool apMode = true;
const char *apWifiSsid = "Smart Lamp";

String header;

String currentLampState = "off";

// SECTION: -- Webserver Varables --
#define ServerPort 80
AsyncWebServer webserver(ServerPort);

// View-Engine Token PARAM names:
const char *PARAM_INT1 = "inpName";
const char *PARAM_INT2 = "inpR";
const char *PARAM_INT3 = "inpG";
const char *PARAM_INT4 = "inpB";
const char *PARAM_INT5 = "inpH";
const char *PARAM_INT6 = "selM";
const char *PARAM_INT7 = "conSta";
const char *PARAM_INT8 = "inpWID";
const char *PARAM_INT9 = "inpWPa";
const char *PARAM_INT10 = "inpBIP";
const char *PARAM_INT11 = "inpBUs";
const char *PARAM_INT12 = "inpBPa";

const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE HTML><html><head><title>Smart Lamp</title><meta name="viewport" content="width=device-width, initial-scale=1"><meta charset="UTF-8"><link rel="stylesheet" type="text/css" href="https://lhenryk.github.io/smart-lamp-assets/css/smart-lamp_web-style.css"/><script src="https://lhenryk.github.io/smart-lamp-assets/js/smart-lamp_web-script.js"></script></head><body><div class="wrapper"><h1 class="title">Smart Lamp</h1><h1>%inpName%</h1><hr class="p30"><h2>Color Mix</h2><div class="con ver"><div class="box"><form action="/get" target="hidden-form"><div class="con">Red:<input type="range" name="inpR" min="0" max="255" value="%inpR%" class="slider" onchange="this.form.submit();update()">%inpR%</div></form></div><div class="box"><form action="/get" target="hidden-form"><div class="con">Green:<input type="range" name="inpG" min="0" max="255" value="%inpG%" class="slider" onchange="this.form.submit();update()">%inpG%</div></form></div><div class="box"><form action="/get" target="hidden-form"><div class="con">Blue:<input type="range" name="inpB" min="0" max="255" value="%inpB%" class="slider" onchange="this.form.submit();update()">%inpB%</div></form></div><div class="box"><form action="/get" target="hidden-form"><div class="con">Brightness:<input type="range" name="inpH" min="0" max="255" value="%inpH%" class="slider" onchange="this.form.submit();update()">%inpH%</div></form></div></div><hr class="p45"><h2>LED Mode:</h2><form action="/get" target="hidden-form"><select class="inp" name="selM" id="selM" onchange="this.form.submit();update()"><option value="select">Select</option><option value="color">Color</option><option value="anim">Animations</option></select></form><a class="btn" href="/settings">Settings</a><hr class="p70"><h4 style="margin: 0">Smart Lamp</h4><h4 style="margin: 0">Developed by Linus H. Kroog</h4><hr class="p60"></div><iframe style="display:none" name="hidden-form"></iframe></body></html>)rawliteral";
const char nocon_html[] PROGMEM = R"rawliteral(<!DOCTYPE HTML><html><head><title>Smart Lamp</title><meta name="viewport" content="width=device-width, initial-scale=1"><meta charset="UTF-8"><script>function update(){setTimeout(function () { document.location.reload(false); }, 500);}</script><style>body {font-family: 'Courier New', monospace;color: #fff;font-weight: bold;background-color: #233a77;}div {margin: 5px;padding: 15px;border-radius: 5px;}input[type=range] {writing-mode: vertical-lr;direction: rtl;vertical-align: middle;}input[type=submit] {cursor: pointer;}hr {color: #fff;margin: 1rem;}hr.p70 {width: 70%;}hr.p60 {width: 60%;margin-bottom: 8rem;}h1 {margin: 0.5rem;}.title {margin-bottom: 2.5rem;font-size: 2.5rem;}.wrapper {display: flex;flex-direction: column;align-items: center;}.box {margin: 0.5rem;padding: 2rem;display: flex;flex-direction: column;align-items: center;background: #2a3869;border: 2px solid #fff;border-radius: 5px;}.btn {margin: 15px;padding: 0.75rem 0.5rem;color: #fff;background: #2a3869;text-decoration: none;font-size: 1.2rem;font-weight: bold;border: 2px solid #fff;border-radius: 5px;}.btn:hover {color: #233a77;background: white;transition: 0.5s;}.red {background: #dc3545;}.inp {margin: 5px;padding: 5px;color: #fff;background: none;border: 1px solid #fff;border-radius: 5px;}</style></head><body><div class="wrapper"><h1 class="title">Smart Lamp</h1><div class="red">Your lamp is currently not connected to the Internet.<br>Therefore, you should enter the network name and<br>network password of your WiFi below and then restart<br>Smart Lamp by clicking on the "Restart" button.</div><h2>WLAN:</h2><div class="box"><form action="/get" target="hidden-form">SSID:<div><input class="inp" type="text" name="inpWID" value="%inpWID%"><input class="inp" type="submit" value="Save" onclick="update()"></div></form><form action="/get" target="hidden-form">Passwort:<div><input class="inp" type="text" name="inpWPa"><input class="inp" type="submit" value="Save" onclick="update()"></div></form></div><a class="btn red" href="/restart">Restart</a><hr class="p70"><h3 style="margin: 0">Smart Lamp</h3><h4 style="margin: 0">Developed by Linus H. Kroog</h4><hr class="p60"></div><iframe style="display:none" name="hidden-form"></iframe></body></html>)rawliteral";
const char settings_html[] PROGMEM = R"rawliteral(<!DOCTYPE HTML><html><head><title>Smart Lamp</title><meta name="viewport" content="width=device-width, initial-scale=1"><meta charset="UTF-8"><link rel="stylesheet" type="text/css" href="https://lhenryk.github.io/smart-lamp-assets/css/smart-lamp_web-style.css"/><script src="https://lhenryk.github.io/smart-lamp-assets/js/smart-lamp_web-script.js"></script></head><body><div class="wrapper"><h1 class="title">Smart Lamp</h1><h2>Settings</h2><hr class="p30"><div class="container"><h2>General:</h2><div class="box"><form action="/get" target="hidden-form">Lamp name:<div><input class="inp" type="text" name="%inpName%" value="%inpName%"><input class="inp" type="submit" value="Save" onclick="update()"></div></form></div><h2>WLAN:</h2><div class="box"><form action="/get" target="hidden-form">Verbindungsstatus: %conSta%<br>SSID:<div><input class="inp" type="text" name="%inpWID%" value="%inpWID%"><input class="inp" type="submit" value="Save" onclick="update()"></div></form><form action="/get" target="hidden-form">Passwort:<div><input class="inp" type="text" name="%inpWPa%"><input class="inp" type="submit" value="Save" onclick="update()"></div></form></div><h2>Mqtt:</h2><div class="box"><form action="/get" target="hidden-form">Broker IP:<div><input class="inp" type="text" name="%inpBIP%" value="%inpBIP%"><input class="inp" type="submit" value="Save" onclick="update()"></div></form><form action="/get" target="hidden-form">Username:<div><input class="inp" type="text" name="%inpBUs%" value="%inpBUs%"><input class="inp" type="submit" value="Save" onclick="update()"></div></form><form action="/get" target="hidden-form">Passwort:<div><input class="inp" type="text" name="%inpBPa%"><input class="inp" type="submit" value="Save" onclick="update()"></div></form></div><h2>More:</h2><div class="box"><a class="btn red" href="/restart">Restart</a></div></div><hr class="p45"><a class="btn" href="/">Back</a><hr class="p70"><h4 style="margin: 0">Smart Lamp</h4><h4 style="margin: 0">Developed by Linus H. Kroog</h4><hr class="p60"></div><iframe style="display:none" name="hidden-form"></iframe></body></html>)rawliteral";

//
// SECTION: -- Graphic Varables --
int currentSeg = 0;
int currentLed = 0;

int color[3];

// Pixel buffers:
int pixelHidBuffer[6][7][3];
int pixelOutBuffer[6][7][3];
int emtyBuffer[6][7][3];

// Create neopixel obj
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PixelNum, PIN, NEO_GRB + NEO_KHZ800);

//
//
//
// SECTION: --- Function declarations: ---
//

// SECTION: -- General --
void serialInitialisationOutput();
void checkCurrentLEDMode();

// SECTION: -- Rotary Encoder --
void checkRotaryEncoderStates();
void rotaryEncoderButtonStateHandler();

// SECTION: -- Access Point --
void checkClientConnection();

// SECTION: -- Webserver --
void setupWebserver();
void notFound(AsyncWebServerRequest *request);
void clientIndexRequestHandler(AsyncWebServerRequest *request);
void clientSettingsRequestHandler(AsyncWebServerRequest *request);
void clientRestartRequestHandler(AsyncWebServerRequest *request);
void clientGetRequestHandler(AsyncWebServerRequest *request);
void webserverLoop();

// SECTION: -- Lamp Mode --

// SECTION: -- Animation functions: --
void whiteModeAnimation();

// SECTION: -- Graphic --
void copyPixelBufferInOut();
void copyPixelBufferInHid();

// TODO: implementation of shiftRowRight(int rowIndex, int coloumShifts);
void shiftRowRight(int rowIndex, int coloumShifts);
void shiftRowLeft(int rowIndex, int coloumShifts);

void showLampColor(int r, int g, int b, int brightness);

void showLedMatrix();

void setSegmentLed(int segmentId = 0, int ledId = 0, int r = 0, int g = 0, int b = 0);

void dumpPixelHidBuffer();
void dumpPixelOutBuffer();

// TODO: Check existance of function:
void updateSegment();

//
//
// SECTION: --- Helper Functions ---
//

String readFile(fs::FS &fs, const char *path)
{
  // Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if (!file || file.isDirectory())
  {
    // Serial.println("- empty file or failed to open file");
    return String();
  }
  // Serial.println("- read from file:");
  String fileContent;
  while (file.available())
  {
    fileContent += String((char)file.read());
  }
  file.close();
  // Serial.println(fileContent);
  return fileContent;
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
  // Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if (!file)
  {
    // Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    // Serial.println("- file written");
  }
  else
  {
    // Serial.println("- write failed");
  }
  file.close();
}

String processRequest(const String &var)
{
  if (var == PARAM_INT1)
  {
    return tempVarInpName;
  }
  else if (var == PARAM_INT2)
  {
    Serial.println(tempVarInpR);
    return tempVarInpR;
  }
  else if (var == PARAM_INT3)
  {
    return tempVarInpG;
  }
  else if (var == PARAM_INT4)
  {
    return tempVarInpB;
  }
  else if (var == PARAM_INT5)
  {
    return tempVarInpH;
  }
  else if (var == PARAM_INT6)
  {
    return tempVarSelM;
  }
  else if (var == PARAM_INT7)
  {
    return tempVarConSta;
  }
  else if (var == PARAM_INT8)
  {
    return tempVarInpWID;
  }
  else if (var == PARAM_INT9)
  {
    return tempVarInpWPa;
  }
  else if (var == PARAM_INT10)
  {
    return tempVarInpBIP;
  }
  else if (var == PARAM_INT11)
  {
    return tempVarInpBUs;
  }
  else if (var == PARAM_INT12)
  {
    return tempVarInpBPa;
  }
  return String();
}