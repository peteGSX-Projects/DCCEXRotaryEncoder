/*
 *  © 2022 Peter Cole
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
GC9A01 round 240x240 LCD display idea:

Outer circle representing turntable surrounds with each position marked.
Internal line representing turntable with home end marked.
config.h or similar used to define the various positions using angles:
- Home angle specified
- Each position in degrees from home
Rotating encoder rotates turntable, blue colour to indicate desire to move
Push button sets orange flashing to indicate turntable is moving
When response from CS received move has finished, stop flashing and set green
When rotating encoder, alignment with a position highlights it somehow
Need to devise a formula to use for rotating the turntable in line with the encoder
Feedback from EX-CS should probably just be SET(vpin) so no extra commands are needed,
just need to update IO_RotaryEncoder.h to accept and pass that on.
*/

/*
Include Arduino platform library.
*/
#include <Arduino.h>

/*
Create a struct to define turntable positions (defined in positions.h).
*/
typedef struct {
  uint16_t angle;
  uint8_t positionId;
  char description[11];
} positionDefinition;

/*
If we haven't got a custom config.h, use the example.
*/
#if __has_include ( "config.h")
  #include "config.h"
#else
  #warning config.h not found. Using defaults from config.example.h
  #include "config.example.h"
#endif

/*
Generate comiler error if no display in use.
*/
#ifndef USE_OLED
#ifndef USE_GC9A01
#error No display defined, specify either OLED or GC9A01
#endif
#endif

/*
If GC9A01 defined, include the necessary library and files.
*/
#ifdef USE_GC9A01
#ifdef USE_OLED
// If we've defined both display options, we need to bail out compiling.
#error USE_GC9A01 and USE_OLED defined, must only specify one display option
#endif
// If we haven't got a custom positions.h, use the example.
#if __has_include ( "positions.h")
  #include "positions.h"
#else
  #warning positions.h not found. Using defaults from positions.example.h
  #include "positions.example.h"
#endif

// If we haven't got a custom colours.h, use the example.
#if __has_include ( "colours.h")
  #include "colours.h"
#else
  #warning colours.h not found. Using defaults from colours.example.h
  #include "colours.example.h"
#endif
#include <Arduino_GFX_Library.h>
#endif

/*
If OLED defined, include the required libraries.
*/
#ifdef USE_OLED
#ifdef USE_GC9A01
// If we've defined both display options, we need to bail out compiling.
#error USE_GC9A01 and USE_OLED defined, must only specify one display option
#endif
#include <SPI.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiSpi.h"
#endif

/*
Include required libraries and files.
*/
#include "avdweb_Switch.h"
#include "Rotary.h"
#include "Wire.h"
#include "version.h"

/*
Global variables for the rotary encoder.
*/
bool encoderRead = true;    // Allows encoder to be rotated without updating position
int8_t counter = 0;         // Counter to be incremented/decremented by rotation
int8_t position = 0;        // Position sent to the CommandStation

/*
Instantiate our rotary encoder and switch objects.
*/
Rotary rotary = Rotary(ROTARY_DT, ROTARY_CLK);
Switch encoderButton(ROTARY_BTN, INPUT_PULLUP, POLARITY, DEBOUNCE, LONG_PRESS);

/*
Global variables, objects, and functions specifically for OLED.
*/
#ifdef USE_OLED
/*
Instantiate the OLED object.
*/
SSD1306AsciiSpi oled;

/*
Function to display the currently selected position of the encoder.
Set this by a button press, or 0 on startup.
*/
void displaySelectedPosition(int8_t position) {
  oled.setCursor(0, 0);
  oled.set2X();
  oled.print(F("Current: "));
  oled.print(position);
  oled.clearToEOL();
}

/*
Function to display the position the encoder is moved to.
This should be updated by any encoder movement.
*/
void displayNewPosition(int8_t position) {
  oled.setCursor(0, 3);
  oled.set2X();
  oled.print(F("Move to: "));
  oled.print(position);
  oled.clearToEOL();
}

/*
Function to display the home reset process.
This should be set by a long press.
*/
void displayHomeReset() {
  oled.clear();
  oled.set1X();
  oled.setCursor(0,0);
  oled.println(F("Resetting home position"));
  oled.println(F("Rotate encoder to home"));
  oled.println(F("Press button to confirm"));
}
// End of OLED functions
#endif

/*
Global variables, objects, and functions specifically for GC9A01.
*/
#ifdef USE_GC9A01
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

// Define radians for angle calculation
#define ONE_DEGREE_RADIAN 0.01745329
#define RIGHT_ANGLE_RADIAN 1.57079633

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

void drawTurntable(uint16_t angle) {
  float x, y;
  int16_t x0, x1, y0, y1, homeEnd, otherEnd, indicatorInner, indicatorOuter, hx0, hy0, hx1, hy1;
  uint16_t homeEndColour = TURNTABLE_HOME_COLOUR;
  bool updateText = false;
  for (uint8_t i = 0; i < NUMBER_OF_POSITIONS; i++) {
    if (angle == turntablePositions[i].angle || angle == HOME_ANGLE) {
      homeEndColour = HOME_HIGHLIGHT_COLOUR;
      updateText = true;
    }
  }
  if (updateText) {
    drawPositionText(angle, true);
    drawPositionText(angle, false);
  } else {
    drawPositionText(angle, true);
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
  gfx->drawLine(x0, y0, x1, y1, TURNTABLE_COLOUR);
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

// End of GC9A01 functions
#endif

void setup() {
  Serial.begin(115200);
  Serial.print(F("DCC-EX Rotary Encoder "));
  Serial.println(VERSION);
  Serial.print(F("Available at I2C address 0x"));
  Serial.println(I2C_ADDRESS, HEX);
#ifdef USE_OLED
  oled.begin(&SH1106_128x64, OLED_CS, OLED_DC);
  oled.setFont(Callibri11);
  oled.clear();
  oled.println(F("DCC-EX Rotary Encoder"));
  oled.print(F("Version: "));
  oled.println(VERSION);
  oled.print(F("I2C Address: 0x"));
  oled.println(I2C_ADDRESS, HEX);
  delay(2000);
  oled.clear();
  displaySelectedPosition(counter);
#endif
#ifdef USE_GC9A01
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
  pitRadius = displayCentre - PIT_OFFSET;
  turntableLength = (pitRadius - 5) * 2;
  gfx->drawCircle(displayCentre, displayCentre, pitRadius, PIT_COLOUR);
  drawPositionMarks();
  drawTurntable(turntableAngle);
#endif
  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(requestEvent);
}

void loop() {
  encoderButton.poll();
  if (encoderButton.longPress()) {
    // Disable reading position allow rotation to "home"
    encoderRead = false;
    Serial.println(F("Disabling position counts"));
#ifdef USE_OLED
    displayHomeReset();
#endif
  } else if (encoderButton.singleClick() && encoderRead) {
    position = counter;
    Serial.print(F("Sending position "));
    Serial.print(position);
    Serial.println(F(" to CommandStation"));
#ifdef USE_OLED
    displaySelectedPosition(position);
#endif
  } else if (encoderButton.singleClick() && !encoderRead) {
    // Once rotated to "home", zero counter and enable again
    counter = 0;
    encoderRead = true;
    Serial.println(F("Enabling position counts"));
#ifdef USE_OLED
    displaySelectedPosition(position);
#endif
  }
  if (encoderRead) {
    unsigned char result = rotary.process();
    if (result == DIR_CW) {
      if (counter < 127) {
        counter++;
      }
    } else if (result == DIR_CCW) {
      if (counter > -127) {
        counter--;
      }
    }
#ifdef DIAG
    Serial.println(counter);
#endif
#ifdef USE_OLED
    displayNewPosition(counter);
#endif
  }
}

void requestEvent() {
  Wire.write(position);
}