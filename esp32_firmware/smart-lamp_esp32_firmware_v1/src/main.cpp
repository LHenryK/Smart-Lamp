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

  // attachInterrupt(digitalPinToInterrupt(rotaryButtonPin), rotaryEncoderButtonStateHandler, CHANGE); // Sobald sich der Status (CHANGE) des Interrupt Pins (SW = D2) ändern, soll der Interrupt Befehl (onInterrupt)ausgeführt werden.

  // Initalise neopixel communication
  Serial.print("SETUP: Neopixels ...");
  pixels.begin();
  pixels.setBrightness(currentBrightness);
  Serial.println(" OK!");
  Serial.println();

  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Initalise Wi-Fi network communication
  Serial.println("SETUP: Network Connection:");
  Serial.print(" -> Start AP Mode ");

  WiFi.softAP(apWifiSsid, "");

  Serial.println(" OK!");

  IPAddress IP = WiFi.softAPIP();

  Serial.print(" -> AP IP: ");
  Serial.println(IP);

  // Initalise mDNS Service:
  Serial.print(" -> Start mDNS ");
  if (!MDNS.begin("smart-lamp"))
  { // Set the hostname to "esp32.local"
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
  Serial.print(" OK");

  Serial.print(" -> Port: ");
  Serial.println(ServerPort);

  Serial.println();

  // Further setup routine:

  // Test LED Setup:

  /*
  for (int m = 0; m < 6; m++)
  {
    for (int n = 0; n < 12; n++)
    {
      setSegmentLed(m, n, 255, 0, 0);
      pixels.show();
      delay(500);
    }
  }
  */

  // LED Setup:
  color[0] = 0;
  color[1] = 0;
  color[2] = 0;

  for (int i = 0; i < 6; i++)
  {
    if (i % 2 == 0)
    {
      color[0] = 255;
      color[1] = 255;
      color[2] = 255;
    }
    // else if (i % 3 == 0)
    // {
    //   color[0] = 0;
    //   color[1] = 255;
    //   color[2] = 0;
    // }
    // else
    // {
    //   color[0] = 0;
    //   color[1] = 0;
    //   color[2] = 255;
    // }
    for (int j = 0; j < 12; j++)
    {
      pixelHidBuffer[i][j][0] = color[0];
      pixelHidBuffer[i][j][1] = color[1];
      pixelHidBuffer[i][j][2] = color[2];
    }
  }

  dumpPixelHidBuffer();

  copyPixelBufferInOut();

  showLedMatrix();
}

void loop()
{
  // checkClientConnection();
  //  put your main code here, to run repeatedly:
  //  for (int i = 0; i < PixelNum; i++)
  //  {
  //    pixels.setPixelColor(i, 255, 255, 255);
  //    pixels.show();
  //    delay(100);
  //  }

  // for (int l = 0; l <= currentLed; l++)
  // {
  //   setSegmentLed(currentSeg, l, 255, 0, 0);
  //   pixels.show();
  //   delay(100);
  // }

  // currentSeg++;
  // currentLed++;

  // if (currentSeg >= 6)
  // {
  //   currentSeg = 0;
  // }

  // if (currentLed >= pixelSegmentLength)
  // {
  //   currentLed = 0;
  // }

  // int color[] = {255, 0, 255};

  /*
  updateSegment();
  pixels.clear();
  delay(500);
  */

  // for (int k = 0; k < 12; k++)
  // {
  //   shiftRowLeft(k, 1);
  //   copyPixelBufferInHid();
  // }

  // showLedMatrix();
  // delay(250);
  // pixels.clear();
  // pixels.show();
  // delay(150);
  // showLedMatrix();
  // delay(250);
  // pixels.clear();
  // pixels.show();
  // delay(150);

  // EXPERIMENTAL !!!!!
  /*
    // LED Setup:
    color[0] = 0;
    color[1] = 0;
    color[2] = 0;

    for (int i = 0; i < 6; i++)
    {
      if (i % 2 == 0)
      {
        color[0] = 255;
        color[1] = 0;
        color[2] = 0;
      }
      // else if (i % 3 == 0)
      // {
      //   color[0] = 0;
      //   color[1] = 255;
      //   color[2] = 0;
      // }
      else
      {
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
      }
      for (int j = 0; j < 12; j++)
      {
        pixelHidBuffer[i][j][0] = color[0];
        pixelHidBuffer[i][j][1] = color[1];
        pixelHidBuffer[i][j][2] = color[2];
      }
    }

    copyPixelBufferInOut();

    showLedMatrix();
    delay(200);
    pixels.clear();
    pixels.show();
    delay(100);

    showLedMatrix();
    delay(200);
    pixels.clear();
    pixels.show();
    delay(100);

    // LED Setup:
    color[0] = 0;
    color[1] = 0;
    color[2] = 0;

    for (int i = 0; i < 6; i++)
    {
      if (i % 2 == 0)
      {
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
      }
      // else if (i % 3 == 0)
      // {
      //   color[0] = 0;
      //   color[1] = 255;
      //   color[2] = 0;
      // }
      else
      {
        color[0] = 0;
        color[1] = 0;
        color[2] = 255;
      }
      for (int j = 0; j < 12; j++)
      {
        pixelHidBuffer[i][j][0] = color[0];
        pixelHidBuffer[i][j][1] = color[1];
        pixelHidBuffer[i][j][2] = color[2];
      }
    }

    copyPixelBufferInOut();

    showLedMatrix();
    delay(200);
    pixels.clear();
    pixels.show();
    delay(100);

    showLedMatrix();
    delay(200);
    pixels.clear();
    pixels.show();
    delay(100);

  */
  // EXPERIMENTAL END !!!!!

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
  // checkRotaryEncoderStates();

  // webserverLoop();

  showLampColor(currentRChannel, currentGChannel, currentBChannel, currentBrightness);

  // checkCurrentLEDMode();

  pixels.setBrightness(currentBrightness);
  showLedMatrix();
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

void checkRotaryEncoderStates()
{
  // TODO: Remove the following commented lines for reading the current rotaryEncoder States
  // bool rotaryPulseForwards = digitalRead(2);
  // bool rotaryPulseBackwards = digitalRead(4);

  // if (rotaryPulseForwards == HIGH && isRotaryForwardsHigh != true)
  // {
  //   Serial.println("PIN: 2, HIGH");

  //   if (currentBrightness < 255)
  //   {
  //     currentBrightness += 10;
  //     Serial.print("Brightness: ");
  //     Serial.println(currentBrightness);
  //   }
  //   isRotaryForwardsHigh = true;
  // }
  // else if (rotaryPulseForwards == LOW)
  // {
  //   isRotaryForwardsHigh = false;
  // }

  // if (rotaryPulseBackwards == HIGH && isRotaryBackwardsHigh != true)
  // {
  //   Serial.println("PIN: 4, HIGH");

  //   if (currentBrightness > 1)
  //   {
  //     currentBrightness -= 10;
  //     Serial.print("Brightness: ");
  //     Serial.println(currentBrightness);
  //   }
  //   isRotaryBackwardsHigh = true;
  // }
  // else if (rotaryPulseBackwards == LOW)
  // {
  //   isRotaryBackwardsHigh = false;
  // }

  // TODO: Improve following lines for the rotaryEncoder BTN for memorizing set and release states!
  // Check if button toggled the isMenuOpen state:
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
  request->send_P(200, "text/html", index_html, processor);
}

void clientGetRequestHandler(AsyncWebServerRequest *request)
{
  String inputMessage;
  // GET inputString value on <ESP_IP>/get?inputString=<inputMessage>
  if (request->hasParam(PARAM_INT1))
  {
    inputMessage = request->getParam(PARAM_INT1)->value();
    writeFile(SPIFFS, "/inpR.txt", inputMessage.c_str());
  }
  // GET inputInt value on <ESP_IP>/get?inputInt=<inputMessage>
  else if (request->hasParam(PARAM_INT2))
  {
    inputMessage = request->getParam(PARAM_INT2)->value();
    writeFile(SPIFFS, "/inpG.txt", inputMessage.c_str());
  }
  // GET inputFloat value on <ESP_IP>/get?inputFloat=<inputMessage>
  else if (request->hasParam(PARAM_INT3))
  {
    inputMessage = request->getParam(PARAM_INT3)->value();
    writeFile(SPIFFS, "/inpB.txt", inputMessage.c_str());
  }
  else if (request->hasParam(PARAM_INT4))
  {
    inputMessage = request->getParam(PARAM_INT4)->value();
    writeFile(SPIFFS, "/inpH.txt", inputMessage.c_str());
  }
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
  int inputR = readFile(SPIFFS, "/inpR.txt").toInt();
  // Serial.print("*** Your InputR: ");
  // Serial.println(inputR);
  currentRChannel = inputR;

  int inputG = readFile(SPIFFS, "/inpG.txt").toInt();
  // Serial.print("*** Your InputG: ");
  // Serial.println(inputG);
  currentGChannel = inputG;

  int inputB = readFile(SPIFFS, "/inpB.txt").toInt();
  // Serial.print("*** Your InputB: ");
  // Serial.println(inputB);
  currentBChannel = inputB;

  int inputH = readFile(SPIFFS, "/inpH.txt").toInt();
  // Serial.print("*** Your InputH: ");
  // Serial.println(inputH);
  currentBrightness = inputH;
}

// TODO: Webserver Request Handler:

//
//
// SECTION: -- Graphics functions: --
//
void copyPixelBufferInOut()
{
  for (int i = 0; i < 6; i++)
  {
    for (int j = 0; j < 12; j++)
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
    for (int j = 0; j < 12; j++)
    {
      for (int k = 0; k < 3; k++)
      {
        pixelHidBuffer[i][j][k] = pixelOutBuffer[i][j][k];
      }
    }
  }
}

void shiftRowLeft(int rowIndex, int coloumShifts)
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
}

// TODO: Implement function that shifts right!

void showLampColor(int r, int g, int b, int brightness)
{
  // LED Setup:
  color[0] = r;
  color[1] = g;
  color[2] = b;

  for (int i = 0; i < 6; i++)
  {
    for (int j = 0; j < 12; j++)
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
    for (int j = 0; j < 12; j++)
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
    for (int j = 0; j < 12; j++)
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
    for (int j = 0; j < 12; j++)
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