/*
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
#define PixelNum 72

int rotaryForwardsPin = 2;
int rotaryBackwardsPin = 4;
int rotaryButtonPin = 27;

long altePosition = -999;

RotaryEncoder encoder(rotaryBackwardsPin, rotaryForwardsPin, RotaryEncoder::LatchMode::TWO03);

unsigned long serialBaud = 115200;

int currentRChannel = 255;
int currentGChannel = 255;
int currentBChannel = 255;

int currentBrightness = 0;

int pixelSegmentLength = 12;

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
const char *apWifiSsid = "ESP Smart Lamp";

String header;

String currentLampState = "off";

// SECTION: -- Webserver Varables --
#define ServerPort 80
AsyncWebServer webserver(ServerPort);

const char *PARAM_INT1 = "inputIntR";
const char *PARAM_INT2 = "inputIntG";
const char *PARAM_INT3 = "inputIntB";
const char *PARAM_INT4 = "inputIntH";

/*
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Smart Lamp</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="UTF-8">
  <script>
    function update() {
      setTimeout(function(){ document.location.reload(false); }, 500);
    }
  </script>
  <style>
  body {
    font-family: 'Courier New', monospace;
    background-color: #233a77;
    color: #fff;
  }
  .wrapper {
    display: flex;
    flex-direction: column;
    align-items: center;
  }
  .wrapper .slider {
    color: #fff;
  }
  </style></head><body>
  <div class="wrapper">
  <h1>ESP - Smart Lamp</h1>
  <form action="/get" target="hidden-form">
    Rot (Wert: %inputIntR%): <input type="range" name="inputIntR" min="0" max="255" value="%inputIntR%" class="slider" onchange="this.form.submit();update();"/>
  </form>
  <form action="/get" target="hidden-form">
    Grün (Wert: %inputIntG%): <input type="range" name="inputIntG" min="0" max="255" value="%inputIntG%" class="slider" onchange="this.form.submit();update();"/>
  </form>
  <form action="/get" target="hidden-form">
    Blau (Wert: %inputIntB%): <input type="range" name="inputIntB" min="0" max="255" value="%inputIntB%" class="slider" onchange="this.form.submit();update();"/>
  </form>
  <form action="/get" target="hidden-form">
    Helligkeit (Wert: %inputIntH%): <input type="range" name="inputIntH" min="0" max="255" value="%inputIntH%" class="slider" onchange="this.form.submit();update();"/>
  </form>
  <details>
  <summary style="justify-content: space-between"><p>WLAN Einstellungen:</p> <button onclick="this.details.disable=true"></summary>
  <form action="/get" target="hidden-form">
    SSID: <input type="text" name="input1">
    <input type="submit" value="Submit">
  </form>
  <form action="/get" target="hidden-form">
    Passwort: <input type="text" name="input1">
    <input type="submit" value="Submit">
  </form>
  </details>
  <h3>Developed by Linus H. Kroog</h3>
  <iframe style="display:none" name="hidden-form"></iframe>
  </div>
</body></html>)rawliteral";
*/

const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE HTML><html><head><title>ESPSmartLamp</title><meta name="viewport" content="width=device-width,initial-scale=1"><meta charset="UTF-8"><script>function update(){setTimeout(function(){document.location.reload(false);},500)}</script><style>body{font-family:'CourierNew',monospace;background-color:#233a77;color:#fff}.wrapper{display:flex;flex-direction:column;align-items:center}details summary{font-size:1.5rem;font-weight:bold;margin:10px;}</style></head><body><div class="wrapper"><h1>SmartLamp</h1><form action="/get" target="hidden-form">Rot(Wert: %inputIntR%):<input type="range" name="inputIntR" min="0" max="255" value="%inputIntR%" class="slider" onchange="this.form.submit();update()"></form><form action="/get" target="hidden-form">Grün(Wert: %inputIntG%):<input type="range" name="inputIntG" min="0" max="255" value="%inputIntG%" class="slider" onchange="this.form.submit();update()"></form><form action="/get" target="hidden-form">Blau(Wert: %inputIntB%):<input type="range" name="inputIntB" min="0" max="255" value="%inputIntB%" class="slider" onchange="this.form.submit();update()"></form><form action="/get" target="hidden-form">Helligkeit(Wert: %inputIntH%):<input type="range" name="inputIntH" min="0" max="255" value="%inputIntH%" class="slider" onchange="this.form.submit();update()"></form><details style="margin-top:4rem"><summary>Einstellungen:</summary><h2>WLAN:</h2><form action="/get" target="hidden-form">SSID:<input type="text" name="input1"><input type="submit"value="Save"></form><form action="/get" target="hidden-form">Passwort:<input type="text" name="input1"><input type="submit" value="Save"></form><h2>Mqtt:</h2><form action="/get" target="hidden-form">BrokerIP:<input type="text" name="input1"><input type="submit" value="Save"></form><form action="/get" target="hidden-form">Username:<input type="text" name="input1"><input type="submit" value="Save"></form><form action="/get" target="hidden-form">Passwort:<input type="text" name="input1"><input type="submit" value="Save"></form></details><hr width="90%" style="color:#fff;margin:1rem"><h3 style="margin:0">Developed by Linus H. Kroog</h3><hr width="80%" style="color:#fff;margin:1rem"><iframe style="display:none" name="hidden-form"></iframe></div></body></html>)rawliteral";

//
// SECTION: -- Graphic Varables --
int currentSeg = 0;
int currentLed = 0;

int color[3];

// Pixel buffers:
int pixelHidBuffer[6][12][3];
int pixelOutBuffer[6][12][3];

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
  Serial.println(fileContent);
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
  if (var == "inputIntR")
  {
    return readFile(SPIFFS, "/inputIntR.txt");
  }
  else if (var == "inputIntG")
  {
    return readFile(SPIFFS, "/inputIntG.txt");
  }
  else if (var == "inputIntB")
  {
    return readFile(SPIFFS, "/inputIntB.txt");
  }
  else if (var == "inputIntH")
  {
    return readFile(SPIFFS, "/inputIntH.txt");
  }
  return String();
}