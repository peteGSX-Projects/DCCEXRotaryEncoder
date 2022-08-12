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

#include <Arduino.h>

// If we haven't got a custom config.h, use the example.
#if __has_include ( "config.h")
  #include "config.h"
#else
  #warning config.h not found. Using defaults from config.example.h
  #include "config.example.h"
#endif

#include "avdweb_Switch.h"
#include "Rotary.h"
#include "Wire.h"
#include "version.h"
#include <SPI.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiSpi.h"

/*
Global variable to allow rotation of the encoder back to "home"
without updating the position send to the CS.
*/
bool encoderRead = true;

/*
Instantiate our rotary encoder object.
Rotary encoder is wired with the common to ground and the two
outputs as defined in config.h.
*/
Rotary rotary = Rotary(ROTARY_DT, ROTARY_CLK);

/*
Instantiate our rotary encoder button.
Rotary encoder button wired as defined in config.h.
*/
Switch encoderButton(ROTARY_BTN, INPUT_PULLUP, POLARITY, DEBOUNCE, LONG_PRESS);

/*
Global variable for the counter that will be incremented or
decremented by rotation.
*/
int8_t counter = 0;

/*
Global variable that is set by the encoder button single push
and is sent to the CS when requested.
*/
int8_t position = 0;

/*
Instantiate our OLED object.
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

void setup() {
  Serial.begin(115200);
  oled.begin(&SH1106_128x64, CS_PIN, DC_PIN);
  oled.setFont(Callibri11);
  oled.clear();
  Serial.print(F("DCC-EX Rotary Encoder "));
  Serial.println(VERSION);
  Serial.print(F("Available at I2C address 0x"));
  Serial.println(I2C_ADDRESS, HEX);
  oled.println(F("DCC-EX Rotary Encoder"));
  oled.print(F("Version: "));
  oled.println(VERSION);
  oled.print(F("I2C Address: 0x"));
  oled.println(I2C_ADDRESS, HEX);
  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(requestEvent);
  delay(2000);
  oled.clear();
  displaySelectedPosition(counter);
}

void loop() {
  encoderButton.poll();

  if (encoderButton.longPress()) {
    // Disable reading position allow rotation to "home"
    encoderRead = false;
    Serial.println(F("Disabling position counts"));
    displayHomeReset();
  } else if (encoderButton.singleClick() && encoderRead) {
    position = counter;
    Serial.print(F("Sending position "));
    Serial.print(position);
    Serial.println(F(" to CommandStation"));
    displaySelectedPosition(position);
  } else if (encoderButton.singleClick() && !encoderRead) {
    // Once rotated to "home", zero counter and enable again
    counter = 0;
    encoderRead = true;
    Serial.println(F("Enabling position counts"));
    displaySelectedPosition(position);
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
    displayNewPosition(counter);
  }
}

void requestEvent() {
  Wire.write(position);
}