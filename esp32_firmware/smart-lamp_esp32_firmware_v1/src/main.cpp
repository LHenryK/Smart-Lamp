/*
 * Smart Lamp - project
 *
 * Author: Linus H. Kroog
 * E-Mail: linus.henry.kroog@bbs-winsen.de
 *
 * File name: main.cpp
 *
 * //TODO: Feature List:
 * //TODO: -> Add animation profile functions
 * //TODO: -> Add MQTT integration with or without SSL certification auth.
 * //TODO: -> Add OTA for doing updates much more simpler
 */

#include <main.h>

void setup()
{
  // Initalise serial communication
  Serial.begin(serialBaud);
  serialInitialisationOutput();

  // Initalise RotaryEncoder
  pinMode(rotaryButtonPin, INPUT); // Hier wird der Interrupt installiert.

  // Initalise neopixel communication
  Serial.print("SETUP: Neopixels ...");
  pixels.begin();
  pixels.setBrightness(currentBrightness);
  Serial.println(" OK!");
  Serial.println();

  Serial.print("SETUP: SPIFFS ...");
  if (!SPIFFS.begin(true))
  {
    Serial.println();
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  else
  {
    Serial.println(" OK");
    Serial.println();
  }

  // Preload temporary varables
  Serial.print("Preload temp varables ...");

  String tempVarInpName = readFile(SPIFFS, "/inpName.txt");

  String tempVarInpR = readFile(SPIFFS, "/inpR.txt");
  String tempVarInpG = readFile(SPIFFS, "/inpG.txt");
  String tempVarInpB = readFile(SPIFFS, "/inpB.txt");
  String tempVarInpH = readFile(SPIFFS, "/inpH.txt");

  String tempVarSelM = readFile(SPIFFS, "/selM.txt");

  String tempVarConSta = readFile(SPIFFS, "/conSta.txt");
  String tempVarInpWID = readFile(SPIFFS, "/inpWID.txt");
  String tempVarInpWPa = readFile(SPIFFS, "/inpWPa.txt");
  String tempVarInpBIP = readFile(SPIFFS, "/inpBIP.txt");
  String tempVarInpBUs = readFile(SPIFFS, "/inpBUs.txt");
  String tempVarInpBPa = readFile(SPIFFS, "/inpBPa.txt");

  Serial.println(" OK");
  Serial.println();

  // Initalise Wi-Fi network communication
  Serial.println("SETUP: Network Connection:");

  // TODO: Check if there are any wifi network credentials available!!!
  // TODO: if there are any, try to connect to network (at least 5 times)
  // TODO: if not or in case of non successfull connection start AP Mode.

  // Check if network credentials are available
  if (tempVarInpWID != "" && tempVarInpWPa != "")
  {
    // Credentials are available!!!
    // Try to connect to network (at least 5 times)
    int connAttems = 0;

    WiFi.mode(WIFI_STA);
    WiFi.begin(tempVarInpWID, tempVarInpWPa);
    Serial.print(" -> Connecting to WiFi: ");
    Serial.print(tempVarInpWID);
    Serial.print(", with password: ");
    Serial.println(tempVarInpWPa);

    while (connAttems < 15)
    {
      if (WiFi.status() == WL_CONNECTED)
      {
        apMode = false;
        break;
      }
      else
      {
        connAttems += 1;
        Serial.print(" -> ");
        Serial.print(connAttems);
        Serial.println("ed attemt fail");
      }
      delay(2000);
    }

    if (apMode)
    {
      Serial.println(" -> Failed to connect to Network");
      Serial.println();
      WiFi.disconnect();
    }
  }

  if (apMode)
  {
    Serial.print(" -> Start AP Mode ");

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(apWifiSsid, "");

    Serial.println(" OK!");

    IPAddress IP = WiFi.softAPIP();

    Serial.print(" -> AP IP: ");
    Serial.println(IP);
  }
  else
  {
    Serial.print(" -> Network IP: ");
    Serial.println(WiFi.localIP());
  }

  // Initalise mDNS Service:
  Serial.print(" -> Start mDNS ");
  if (!MDNS.begin("smart-lamp"))
  {
    Serial.println("Error setting up MDNS responder!");
    while (1)
    {
      Serial.print(".");
      delay(500);
    }
  }

  Serial.println(" OK!");

  Serial.println();

  Serial.println("SETUP: Webserver:");

  Serial.print(" -> Starting ");
  setupWebserver();
  Serial.println(" OK");

  Serial.print(" -> Port: ");
  Serial.println(ServerPort);

  Serial.println();

  // LED Setup:
  color[0] = 255;
  color[1] = 255;
  color[2] = 255;

  for (int i = 0; i < 6; i++)
  {
    for (int j = 0; j < 7; j++)
    {
      pixelHidBuffer[i][j][0] = color[0];
      pixelHidBuffer[i][j][1] = color[1];
      pixelHidBuffer[i][j][2] = color[2];
    }
  }

  dumpPixelHidBuffer();

  copyPixelBufferInOut();

  showLedMatrix();

  delay(500);
}

void loop()
{
  // checkClientConnection();

  // checkCurrentLEDMode();

  // TODO: Put the following lines in to the checkCurrentLEDMode function.
  checkRotaryEncoderStates();

  webserverLoop();

  pixels.setBrightness(currentBrightness);
  showLampColor(currentRChannel, currentGChannel, currentBChannel, currentBrightness);
}

// SECTION: --- Functions: ---

// SECTION: -- General --
void serialInitialisationOutput()
{
  Serial.println("#-----------------------------------------#");
  Serial.println("# Autor: Linus H. Kroog                   #");
  Serial.println("# E-Mail: linus.henry.kroog@bbs-winsen.de #");
  Serial.println("#-----------------------------------------#");
  Serial.println();
}

void checkCurrentLEDMode()
{
  // first check the current LED Mode:
  switch (currentLampMode)
  {
  case NOMODE:
    if (isMenuOpen)
    {
    }
    else
    {
    }
    break;

  case WHITEMODE:
    showLampColor(255, 255, 255, currentBrightness);
    break;

  case PARINGMODE:
    // checkClientConnection();

  default:
    break;
  }
}

// SECTION: -- Rotary Encoder --

// TODO: Update RGB-H temp and stored values when changing!!!
void checkRotaryEncoderStates()
{
  encoder.tick();

  int neuePosition = encoder.getPosition();
  if (altePosition != neuePosition)
  {
    if (altePosition < neuePosition)
    {
      if (neuePosition <= 51)
      {
        currentBrightness = 5 * neuePosition;
        Serial.print("Brightness: ");
        Serial.println(currentBrightness);
      }
      else
      {
        currentBrightness = 255;
        encoder.setPosition(51);
      }
    }
    else if (altePosition > neuePosition)
    {
      if (neuePosition >= 0)
      {
        currentBrightness = 5 * neuePosition;
        Serial.print("Brightness: ");
        Serial.println(currentBrightness);
      }
      else
      {
        currentBrightness = 0;
        encoder.setPosition(0);
      }
    }
    altePosition = neuePosition;
    Serial.println(neuePosition); // ...soll die aktuelle Position im seriellen Monitor ausgegeben werden.
  }

  // TODO: Improve following lines for the rotaryEncoder BTN for memorizing set and release states!
  // Check if button toggled, then update the isMenuOpen state varable:
  bool rotaryPulseButton = digitalRead(rotaryButtonPin);

  if (rotaryPulseButton == HIGH && isRotaryButtonHigh != true)
  {
    Serial.print("PIN: ");
    Serial.print(rotaryButtonPin);
    Serial.println(", HIGH");

    currentMenuToggleTime = millis();
  }
  else if (rotaryPulseButton == LOW && isRotaryButtonHigh == true)
  {
    isRotaryButtonHigh = false;
    if (currentMenuToggleTime >= menuToggleTime)
    {
      isMenuOpen = !isMenuOpen;
      currentMenuToggleTime = 0;
    }
  }
}

void rotaryEncoderButtonStateHandler()
{
  Serial.println("Rotary Encoder Button: PRESSED!!!");
}

// SECTION: -- Wi-Fi functions: --

void setupWiFiConnnection()
{
  if (apWifiSsid == "")
  {
    WiFi.eraseAP();
  }
}

// SECTION: - Webserver functions: -
void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not Found!");
}

void setupWebserver()
{
  webserver.reset();
  // Set Webserver Responses to Client request:
  webserver.on("/", HTTP_GET, clientIndexRequestHandler);
  Serial.print(".");
  webserver.on("/get", HTTP_GET, clientGetRequestHandler);
  Serial.print(".");
  webserver.onNotFound(notFound);
  Serial.print(".");
  webserver.begin();
  Serial.print(".");
}

void clientIndexRequestHandler(AsyncWebServerRequest *request)
{
  if (apMode)
  {
    request->send_P(200, "text/html", nocon_html, processRequest);
  }
  else
  {
    request->send_P(200, "text/html", index_html, processRequest);
  }
}

void clientGetRequestHandler(AsyncWebServerRequest *request)
{
  String inputMessage;
  // GET inputString value on <ESP_IP>/get?inputString=<inputMessage>

  // Name:
  if (request->hasParam(PARAM_INT1))
  {
    inputMessage = request->getParam(PARAM_INT1)->value();
    writeFile(SPIFFS, "/inpName.txt", inputMessage.c_str());
    tempVarInpName = inputMessage;
  }

  // RGB-H values:
  else if (request->hasParam(PARAM_INT2))
  {
    inputMessage = request->getParam(PARAM_INT2)->value();
    writeFile(SPIFFS, "/inpR.txt", inputMessage.c_str());
    tempVarInpR = inputMessage;
  }
  else if (request->hasParam(PARAM_INT3))
  {
    inputMessage = request->getParam(PARAM_INT3)->value();
    writeFile(SPIFFS, "/inpG.txt", inputMessage.c_str());
    tempVarInpG = inputMessage;
  }
  else if (request->hasParam(PARAM_INT4))
  {
    inputMessage = request->getParam(PARAM_INT4)->value();
    writeFile(SPIFFS, "/inpB.txt", inputMessage.c_str());
    tempVarInpB = inputMessage;
  }
  else if (request->hasParam(PARAM_INT5))
  {
    inputMessage = request->getParam(PARAM_INT5)->value();
    writeFile(SPIFFS, "/inpH.txt", inputMessage.c_str());
    tempVarInpH = inputMessage;
    currentBrightness = inputMessage.toInt();
  }

  // Lamp Mode:
  else if (request->hasParam(PARAM_INT6))
  {
    inputMessage = request->getParam(PARAM_INT6)->value();
    writeFile(SPIFFS, "/selM.txt", inputMessage.c_str());
    tempVarSelM = inputMessage;
  }

  // Connection Status:
  else if (request->hasParam(PARAM_INT7))
  {
    inputMessage = request->getParam(PARAM_INT7)->value();
    writeFile(SPIFFS, "/conSta.txt", inputMessage.c_str());
    tempVarConSta = inputMessage;
  }

  // Wifi:
  else if (request->hasParam(PARAM_INT8))
  {
    inputMessage = request->getParam(PARAM_INT8)->value();
    writeFile(SPIFFS, "/inpWID.txt", inputMessage.c_str());
    tempVarInpWID = inputMessage;
  }
  else if (request->hasParam(PARAM_INT9))
  {
    inputMessage = request->getParam(PARAM_INT9)->value();
    writeFile(SPIFFS, "/inpWPa.txt", inputMessage.c_str());
    tempVarInpWPa = inputMessage;
  }

  // MQTT Broker:
  else if (request->hasParam(PARAM_INT10))
  {
    inputMessage = request->getParam(PARAM_INT10)->value();
    writeFile(SPIFFS, "/inpBIP.txt", inputMessage.c_str());
    tempVarInpBIP = inputMessage;
  }
  else if (request->hasParam(PARAM_INT11))
  {
    inputMessage = request->getParam(PARAM_INT11)->value();
    writeFile(SPIFFS, "/inpBUs.txt", inputMessage.c_str());
    tempVarInpBUs = inputMessage;
  }
  else if (request->hasParam(PARAM_INT12))
  {
    inputMessage = request->getParam(PARAM_INT12)->value();
    writeFile(SPIFFS, "/inpBPa.txt", inputMessage.c_str());
    tempVarInpBPa = inputMessage;
  }

  // No Message:
  else
  {
    inputMessage = "No message sent";
  }
  // Serial.println(inputMessage);
  request->send(200, "text/text", inputMessage);
}

void webserverLoop()
{
  // To access your stored values on inputString, inputInt, inputFloat
  int inputR = tempVarInpR.toInt();
  // Serial.print("*** Your InputR: ");
  // Serial.println(inputR);
  currentRChannel = inputR;

  int inputG = tempVarInpG.toInt();
  // Serial.print("*** Your InputG: ");
  // Serial.println(inputG);
  currentGChannel = inputG;

  int inputB = tempVarInpB.toInt();
  // Serial.print("*** Your InputB: ");
  // Serial.println(inputB);
  currentBChannel = inputB;

  // int inputH = tempVarInpH.toInt();
  // Serial.print("*** Your InputH: ");
  // Serial.println(inputH);
  // currentBrightness = inputH;
}

//
//
// SECTION: -- Graphics functions: --
//
void copyPixelBufferInOut()
{
  for (int i = 0; i < 6; i++)
  {
    for (int j = 0; j < 7; j++)
    {
      for (int k = 0; k < 3; k++)
      {
        pixelOutBuffer[i][j][k] = pixelHidBuffer[i][j][k];
      }
    }
  }
}

void copyPixelBufferInHid()
{
  for (int i = 0; i < 6; i++)
  {
    for (int j = 0; j < 7; j++)
    {
      for (int k = 0; k < 3; k++)
      {
        pixelHidBuffer[i][j][k] = pixelOutBuffer[i][j][k];
      }
    }
  }
}

void shiftRowRight(int rowIndex, int coloumShifts)
{
  for (int i = coloumShifts; i < 6; i++)
  {
    pixelOutBuffer[i][rowIndex][0] = pixelHidBuffer[i - coloumShifts][rowIndex][0];
    pixelOutBuffer[i][rowIndex][1] = pixelHidBuffer[i - coloumShifts][rowIndex][1];
    pixelOutBuffer[i][rowIndex][2] = pixelHidBuffer[i - coloumShifts][rowIndex][2];
  }
  for (int j = 0; j < coloumShifts; j++)
  {
    pixelOutBuffer[j][rowIndex][0] = pixelHidBuffer[6 - coloumShifts + j][rowIndex][0];
    pixelOutBuffer[j][rowIndex][1] = pixelHidBuffer[6 - coloumShifts + j][rowIndex][1];
    pixelOutBuffer[j][rowIndex][2] = pixelHidBuffer[6 - coloumShifts + j][rowIndex][2];
  }

  copyPixelBufferInHid();
}

// TODO: Implement function that shifts right!

void shiftRowLeft(int rowIndex, int coloumShifts)
{
  for (int i = 5 - coloumShifts; i >= 0; i--)
  {
    pixelOutBuffer[i][rowIndex][0] = pixelHidBuffer[i + coloumShifts][rowIndex][0];
    pixelOutBuffer[i][rowIndex][1] = pixelHidBuffer[i + coloumShifts][rowIndex][1];
    pixelOutBuffer[i][rowIndex][2] = pixelHidBuffer[i + coloumShifts][rowIndex][2];
  }
  for (int j = 5; j > 5 - coloumShifts; j--)
  {
    pixelOutBuffer[j][rowIndex][0] = pixelHidBuffer[5 - j][rowIndex][0];
    pixelOutBuffer[j][rowIndex][1] = pixelHidBuffer[5 - j][rowIndex][1];
    pixelOutBuffer[j][rowIndex][2] = pixelHidBuffer[5 - j][rowIndex][2];
  }
  copyPixelBufferInHid();
}

void showLampColor(int r, int g, int b, int brightness)
{
  // LED Setup:
  color[0] = r;
  color[1] = g;
  color[2] = b;

  for (int i = 0; i < 6; i++)
  {
    for (int j = 0; j < 7; j++)
    {
      pixelHidBuffer[i][j][0] = color[0];
      pixelHidBuffer[i][j][1] = color[1];
      pixelHidBuffer[i][j][2] = color[2];
    }
  }

  copyPixelBufferInOut();
  pixels.setBrightness(brightness);
  showLedMatrix();
}

void showLedMatrix()
{
  pixels.clear();
  for (int i = 0; i < 6; i++)
  {
    for (int j = 0; j < 7; j++)
    {
      setSegmentLed(i, j, pixelOutBuffer[i][j][0], pixelOutBuffer[i][j][1], pixelOutBuffer[i][j][2]);
    }
  }
  pixels.show();
}

void setSegmentLed(int segmentId, int ledId, int r, int g, int b)
{
  // pixelHidBuffer[ledId][segmentId][0] = r;
  // pixelHidBuffer[ledId][segmentId][1] = g;
  // pixelHidBuffer[ledId][segmentId][2] = b;

  if (segmentId % 2 == 0)
  {
    pixels.setPixelColor((segmentId * pixelSegmentLength) + ledId, r, g, b);
  }
  else
  {
    pixels.setPixelColor((segmentId * pixelSegmentLength) + pixelSegmentLength - 1 - ledId, r, g, b);
  }
}

void dumpPixelHidBuffer()
{
  Serial.println("### DUMP: hid pixel buffer ###");
  for (int i = 0; i < 6; i++)
  {
    Serial.println();
    Serial.print("SEGMENT ID: ");
    Serial.print(i);
    Serial.println();
    for (int j = 0; j < 7; j++)
    {
      Serial.print("LED ID: ");
      Serial.print(j);
      Serial.print(", [");

      for (int k = 0; k < 3; k++)
      {
        Serial.print(" ");
        Serial.print(pixelHidBuffer[i][j][k]);
        Serial.print(" ");
      }
      Serial.println("]");
    }
  }
}

void dumpPixelOutBuffer()
{
  Serial.println("### DUMP: out pixel buffer ###");
  for (int i = 0; i < 6; i++)
  {
    Serial.println();
    Serial.print("SEGMENT ID: ");
    Serial.print(i);
    Serial.println();
    for (int j = 0; j < 7; j++)
    {
      Serial.print("LED ID: ");
      Serial.print(j);
      Serial.print(", [");

      for (int k = 0; k < 3; k++)
      {
        Serial.print(" ");
        Serial.print(pixelOutBuffer[i][j][k]);
        Serial.print(" ");
      }
      Serial.println("]");
    }
  }
}

// TODO: Remove old verson of update or integrate name into showLEDMatrix()
void updateSegment()
{
  for (int i = 0; i < round(PixelNum / pixelSegmentLength); i++)
  {
    if (i % 2 == 0)
    {
      for (int j = 0; j < pixelSegmentLength; j++)
      {
        pixels.setPixelColor((i * pixelSegmentLength) + j, 255, 0, 0);
        pixels.show();
        delay(100);
      }
    }
    else
    {
      for (int k = pixelSegmentLength; k >= 0; k--)
      {
        pixels.setPixelColor((i * pixelSegmentLength) + k, 0, 255, 0);
        pixels.show();
        delay(100);
      }
    }
  }
}