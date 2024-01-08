/*
 *  © 2024 Peter Cole
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  See <https://www.gnu.org/licenses/>.
*/

/*
Include Arduino platform library.
*/
#include <Arduino.h>
#include "Defines.h"

/*
Include required libraries and files.
*/
#include "avdweb_Switch.h"
#include "Rotary.h"
#include "Wire.h"
#include "Version.h"

/*
Global variables for all modes.
*/
bool encoderRead = true;      // Allows encoder to be rotated without updating position
int8_t counter = 0;           // Counter to be incremented/decremented by rotation
int8_t position = 0;          // Position sent to the CommandStation
bool moving = 0;              // Boolean for moving or not, 1 = moving, 0 = not
const String versionString = VERSION;
char * version;               // Char array to break version into ints
uint8_t versionBuffer[3];     // Buffer to send version to device driver
byte activity;                // Flag to choose what to send to device driver
unsigned long lastBlink = 0;  // Last time display was turned off/on
bool blinkFlag = 0;           // Flag for alternating text clear/colour
bool sendPosition = true;     // Flag for when positions are aligned in turntable mode
uint8_t i2cAddress = I2C_ADDRESS; // Store I2C address in the right type
uint8_t newPosition;          // Variable to store new positions received by the device driver
bool receivedMove = false;    // Boolean to flag if we received a move from the device driver
bool moveTurntable = false;   // Flag the turntable display needs to move

/*
Instantiate our rotary encoder and switch objects.
*/
Rotary rotary = Rotary(ROTARY_DT, ROTARY_CLK);
Switch encoderButton(ROTARY_BTN, INPUT_PULLUP, POLARITY, DEBOUNCE, LONG_PRESS);

/*
Global variables, objects, and functions specifically for GC9A01.
*/
// Static global variables for display parameters
static int16_t displayWidth, displayHeight, displayCentre, turntableLength, pitRadius;
static float turntableDegrees, markDegrees;
// Global variables to facilitate overwriting last position
int16_t lastX0, lastY0, lastX1, lastY1, lastHX0, lastHY0, lastHX1, lastHY1;
uint8_t textX, textY, numChars = 0;
uint16_t turntableAngle = HOME_ANGLE;   // Start display with turntable at home
char textChars[11];     // Stores the current position text

// Instantiate DataBus and GFX objects.
Arduino_DataBus *bus = new Arduino_HWSPI(GC9A01_DC, GC9A01_CS);
Arduino_GFX *gfx = new Arduino_GC9A01(bus, GC9A01_RST, GC9A01_ROTATION, GC9A01_IPS);

/*
Function to display the defined position marks around the pit circle.
*/
void drawPositionMarks() {
  uint8_t homeMarkLength = 12;
  uint8_t positionMarkLength = 10;
  uint16_t markColour;
  float x, y;
  int16_t x0, x1, y0, y1, innerRadius, outerRadius;
#ifndef HOME_ANGLE
#define HOME_ANGLE 0
#endif
  for (uint16_t i = 0; i < 360; i++) {
    bool drawLine = false;
    if (i == HOME_ANGLE) {
      markColour = HOME_COLOUR;
      innerRadius = displayCentre - PIT_OFFSET + 1;
      outerRadius = displayCentre + homeMarkLength - PIT_OFFSET + 1;
      drawLine = true;
    } else {
      for (uint8_t j = 0; j < NUMBER_OF_POSITIONS; j++) {
        if (i == turntablePositions[j].angle) {
          markColour = POSITION_COLOUR;
          innerRadius = displayCentre - PIT_OFFSET + 1;
          outerRadius = displayCentre + positionMarkLength - PIT_OFFSET + 1;
          drawLine = true;
        }
      }
    }
    markDegrees = (ONE_DEGREE_RADIAN * i) - RIGHT_ANGLE_RADIAN;
    x = cos(markDegrees);
    y = sin(markDegrees);
    x0 = x * outerRadius + displayCentre;
    y0 = y * outerRadius + displayCentre;
    x1 = x * innerRadius + displayCentre;
    y1 = y * innerRadius + displayCentre;
    if (drawLine) {
      gfx->drawLine(x0, y0, x1, y1, markColour);
    }
  }
}

/*
Function to display the text for the specified position.
If moving away from a defined position, set the "clear" flag to set to background colour.
*/
void drawPositionText(uint16_t angle, bool clear) {
  uint16_t fontColour;
  gfx->setTextSize(2);
  gfx->setFont();
  if (clear) {
    fontColour = BACKGROUND_COLOUR;
  } else {
    fontColour = POSITION_TEXT_COLOUR;
    numChars = 0;
    for (uint8_t i = 0; i < NUMBER_OF_POSITIONS; i++) {
      if (angle == turntablePositions[i].angle) {
        for (uint8_t j = 0; j < 10; j++) {
          textChars[j] = turntablePositions[i].description[j];
          if (turntablePositions[i].description[j] != '\0') {
            numChars++;
          } else {
            break;
          }
        }
      } else if (angle == HOME_ANGLE) {
        char home[5] = "Home";
        for (uint8_t k = 0; k < 4; k++) {
          textChars[k] = home[k];
        }
        textChars[4] = '\0';
        numChars = 4;
      }
    }
    textX = displayCentre - (numChars / 2 * 10) - 1;
    textY = displayCentre;
  }
  gfx->setTextColor(fontColour);
  gfx->setCursor(textX, textY);
  gfx->print(textChars);
}

/*
Function to draw the turntable at the specified angle.
If the turntable aligns with home or a define position, it will highlight the home end and
display the provided description of the position.
*/
void drawTurntable(uint16_t angle) {
  float x, y;
  int16_t x0, x1, y0, y1, homeEnd, otherEnd, indicatorInner, indicatorOuter, hx0, hy0, hx1, hy1;
  uint16_t homeEndColour = TURNTABLE_HOME_COLOUR;
  bool updateText = false;
  for (uint8_t i = 0; i < NUMBER_OF_POSITIONS; i++) {
    if (angle == turntablePositions[i].angle || angle == HOME_ANGLE) {
      homeEndColour = HOME_HIGHLIGHT_COLOUR;
      updateText = true;
      if (angle == HOME_ANGLE) {
        counter = 0;
      } else {
        counter = turntablePositions[i].positionId;
      }
    }
  }
  if (updateText) {
    drawPositionText(angle, true);
    drawPositionText(angle, false);
    sendPosition = true;
  } else {
    drawPositionText(angle, true);
    sendPosition = false;
  }
  turntableDegrees = (ONE_DEGREE_RADIAN * angle) - RIGHT_ANGLE_RADIAN;
  homeEnd = (turntableLength / 2) - 10;
  otherEnd = - (turntableLength / 2);
  indicatorInner = homeEnd;
  indicatorOuter = indicatorInner + 10;
  x = cos(turntableDegrees);
  y = sin(turntableDegrees);
  x0 = x * homeEnd + displayCentre;
  y0 = y * homeEnd + displayCentre;
  x1 = x * otherEnd + displayCentre;
  y1 = y * otherEnd + displayCentre;
  hx0 = x * indicatorInner + displayCentre;
  hy0 = y * indicatorInner + displayCentre;
  hx1 = x * indicatorOuter + displayCentre;
  hy1 = y * indicatorOuter + displayCentre;
  gfx->drawLine(lastX0, lastY0, lastX1, lastY1, BACKGROUND_COLOUR);
  gfx->drawLine(lastHX0, lastHY0, lastHX1, lastHY1, BACKGROUND_COLOUR);
  if (moving && blinkFlag == 0) {
    gfx->drawLine(x0, y0, x1, y1, BACKGROUND_COLOUR);
  } else {
    gfx->drawLine(x0, y0, x1, y1, TURNTABLE_COLOUR);
  }
  gfx->drawLine(hx0, hy0, hx1, hy1, homeEndColour);
  lastX0 = x0;
  lastY0 = y0;
  lastX1 = x1;
  lastY1 = y1;
  lastHX0 = hx0;
  lastHY0 = hy0;
  lastHX1 = hx1;
  lastHY1 = hy1;
}

/*=============================================================
Function to receive events from the device driver.

RE_RDY - device driver is seeing if the encoder is ready
RE_VER - device driver is requesting the version of the software
RE_READ - device driver is requesting the current position of the encoder
RE_OP - device driver is sending feedback (0 or 1)
RE_MOVE - device driver is sending a new position the encoder didn't initiate
=============================================================*/
void receiveEvent(int receivedBytes) {
  if (receivedBytes == 0) {
    return;
  }
  byte buffer[receivedBytes];
  for (uint8_t byte = 0; byte < receivedBytes; byte++) {
    buffer[byte] = Wire.read();   // Read all received bytes into our buffer array
  }
  switch(buffer[0]) {
    case RE_RDY:
      // Device driver asking if encoder is ready
      if (receivedBytes == 1) {
        activity = RE_RDY;
      }
      break;
    case RE_VER:
      // Device driver asking for version
      if (receivedBytes == 1) {
        activity = RE_VER;
      }
      break;
    case RE_READ:
      // Device driver asking for current position
      if (receivedBytes == 1) {
        activity = RE_READ;
      }
      break;
    case RE_OP:
      // Device driver providing the feedback value (0 or 1)
      if (receivedBytes == 2) {
        moving = buffer[1];
      }
      break;
    case RE_MOVE:
      // Device driver sending a position move
      if (receivedBytes == 2) {
        newPosition = buffer[1];
#ifdef DIAG
        Serial.print(F("Received move to "));
#endif
        // If it's not the current position, flag a change
        if (position != newPosition) {
#ifdef DIAG
          Serial.println(newPosition);
#endif
          receivedMove = true;
        } else {
#ifdef DIAG
          Serial.println("existing position, disregarding");
#endif
          // If it's the same as current position, we don't care
          receivedMove = false;
        }
      }
    default:
      break;
  }
}

/*=============================================================
Function to send data back to the device driver when requested.

Any I2CManager.read() functions will expect responses which are performed here.
=============================================================*/
void requestEvent() {
  if (activity == RE_RDY) {
    // Device driver requested if encoder is ready, send it
    Wire.write(RE_RDY);
  } else if (activity == RE_VER) {
    // Device driver has requested version, sending it
#ifdef DIAG
    Serial.print(F("Send version: v"));
    Serial.print(versionBuffer[0]);
    Serial.print(F("."));
    Serial.print(versionBuffer[1]);
    Serial.print(F("."));
    Serial.println(versionBuffer[2]);
#endif
    Wire.write(versionBuffer, 3);
  } else if (activity == RE_READ) {
    // Device driver requesting current position, send it
    Wire.write(position);
  } else {
    // If anything else is requested, this is an error, send it
    Wire.write(RE_ERR);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.print(F("DCC-EX Rotary Encoder "));
  Serial.println(VERSION);
  Serial.print(F("Available at I2C address 0x"));
  Serial.println(I2C_ADDRESS, HEX);
  // Put version into our array for the query later
  char versionArray[versionString.length() + 1];
  versionString.toCharArray(versionArray, versionString.length() + 1);
  version = strtok(versionArray, "."); // Split version on .
  versionBuffer[0] = atoi(version);   // Major first
  version = strtok(NULL, ".");
  versionBuffer[1] = atoi(version);   // Minor next
  version = strtok(NULL, ".");
  versionBuffer[2] = atoi(version);   // Patch last
  gfx->begin();
  gfx->fillScreen(BACKGROUND_COLOUR);
  pinMode(GC9A01_BL, OUTPUT);
  digitalWrite(GC9A01_BL, HIGH);
  displayWidth = gfx->width();
  displayHeight = gfx->height();
  if (displayWidth < displayHeight)
  {
    displayCentre = displayWidth / 2;
  }
  else
  {
    displayCentre = displayHeight / 2;
  }
  gfx->setTextSize(1);
  gfx->setFont();
  gfx->setTextColor(POSITION_TEXT_COLOUR);
  gfx->setCursor(40, 60);
  gfx->print(F("DCC-EX Rotary Encoder"));
  gfx->setCursor(40, 80);
  gfx->print(F("Version: "));
  gfx->print(versionBuffer[0]);
  gfx->print(F("."));
  gfx->print(versionBuffer[1]);
  gfx->print(F("."));
  gfx->print(versionBuffer[2]);
  gfx->setCursor(40, 100);
  gfx->print(F("I2C Address: 0x"));
  gfx->print(I2C_ADDRESS, HEX);
  delay(2000);
  Wire.begin(i2cAddress);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  gfx->fillScreen(BACKGROUND_COLOUR);
  pitRadius = displayCentre - PIT_OFFSET;
  turntableLength = (pitRadius - 5) * 2;
  gfx->drawCircle(displayCentre, displayCentre, pitRadius, PIT_COLOUR);
  drawPositionMarks();
  drawTurntable(turntableAngle);
}

void loop() {
  encoderButton.poll();
  if (!moving) {
    if (encoderButton.longPress()) {
      // Disable reading position allow rotation to "home"
      encoderRead = false;
      Serial.println(F("Disabling position counts"));
    // If button has been pushed, reading is enabled, and aligned at a position, or if we received a move from the device driver:
    } else if ((encoderButton.singleClick() && encoderRead && sendPosition) || receivedMove) {
      // Update counter to the device driver position
      if (receivedMove) {
        counter = newPosition;
        receivedMove = false;
        moveTurntable = true;
        if (counter == 0) {
          turntableAngle = 0;
        } else {
          for (uint8_t i = 0; i < NUMBER_OF_POSITIONS; i++) {
            if (turntablePositions[i].positionId == counter) {
              turntableAngle = turntablePositions[i].angle;
            }
          }
        }
      }
      position = counter;
      Serial.print(F("Sending position "));
      Serial.print(position);
      Serial.println(F(" to CommandStation"));
    } else if (encoderButton.singleClick() && !encoderRead) {
      // Once rotated to "home", zero counter and enable again
      counter = 0;
      encoderRead = true;
      Serial.println(F("Enabling position counts"));
    }
    if (encoderRead) {
      unsigned char result = rotary.process();
      moveTurntable = false;
      if (result == DIR_CW) {
        if (turntableAngle < 360) {
          turntableAngle++;
        } else {
          turntableAngle = 0;
        }
        moveTurntable = true;
      } else if (result == DIR_CCW) {
        if (turntableAngle > 0) {
          turntableAngle--;
        } else {
          turntableAngle = 359;
        }
        moveTurntable = true;
      }
#ifdef DIAG
      Serial.println(counter);
#endif
      if (moveTurntable || blinkFlag == 0) {
        drawTurntable(turntableAngle);
        blinkFlag = 1;
      }
    }
  } else {
    // Flash position text here
    if (millis() - lastBlink >= BLINK_RATE) {
      blinkFlag = !blinkFlag;
      drawTurntable(turntableAngle);
      lastBlink = millis();
    }
  }
}