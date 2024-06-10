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

bool isMenuOpen = false;
int menuToggleTime = 6000;
int currentMenuToggleTime = 0;

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
const char *apWifiSsid = "Smart Lamp";

String header;

String currentLampState = "off";

// SECTION: -- Webserver Varables --
#define ServerPort 80
AsyncWebServer webserver(ServerPort);

const char *PARAM_INT1 = "inpR";
const char *PARAM_INT2 = "inpG";
const char *PARAM_INT3 = "inpB";
const char *PARAM_INT4 = "inpH";

const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE HTML><html><head><title>Smart Lamp</title><meta name="viewport" content="width=device-width, initial-scale=1"><meta charset="UTF-8"><link rel="stylesheet" type="text/css" href="https://lhenryk.github.io/smart-lamp-assets/css/smart-lamp_web-style.css"/><script src="https://lhenryk.github.io/smart-lamp-assets/js/smart-lamp_web-script.js"></script></head><body><div class="wrapper"><h1 class="title">Smart Lamp</h1><h1>%inpName%</h1><hr class="p30"><h2>Color Mix</h2><div class="con ver"><div class="box"><form action="/get" target="hidden-form"><div class="con">Red:<input type="range" name="inpR" min="0" max="255" value="%inpR%" class="slider" onchange="this.form.submit();update()">%inpR%</div></form></div><div class="box"><form action="/get" target="hidden-form"><div class="con">Green:<input type="range" name="inpG" min="0" max="255" value="%inpG%" class="slider"onchange="this.form.submit();update()">%inpG%</div></form></div><div class="box"><form action="/get" target="hidden-form"><div class="con">Blue:<input type="range" name="inpB" min="0" max="255" value="%inpB%" class="slider" onchange="this.form.submit();update()">%inpB%</div></form></div><div class="box"><form action="/get" target="hidden-form"><div class="con">Brightness:<input type="range" name="inpH" min="0" max="255" value="%inpH%" class="slider" onchange="this.form.submit();update()">%inpH%</div></form></div></div><hr class="p45"><h2>LED Mode:</h2><form action="/get" target="hidden-form"><select class="inp" name="selM" id="selM" onchange="this.form.submit();update()"><option value="color">Color</option><option value="mqtt">MQTT</option><option value="party">Party</option><option value="anim">Animations</option></select></form><a class="btn" href="settings.html">Settings</a><hr class="p70"><h4 style="margin: 0">Smart Lamp</h4><h4 style="margin: 0">Developed by Linus H. Kroog</h4><hr class="p60"></div><iframe style="display:none" name="hidden-form"></iframe></body></html>)rawliteral";

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

String processor(const String &var)
{
  // Serial.println(var);
  if (var == "inpR")
  {
    return readFile(SPIFFS, "/inpR.txt");
  }
  else if (var == "inpG")
  {
    return readFile(SPIFFS, "/inpG.txt");
  }
  else if (var == "inpB")
  {
    return readFile(SPIFFS, "/inpB.txt");
  }
  else if (var == "inpH")
  {
    return readFile(SPIFFS, "/inpH.txt");
  }
  return String();
}