/*
 *  © 2022 Peter Cole
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

bool encoderRead = true;

// Rotary encoder is wired with the common to ground and the two
// outputs to pins 5 and 6.
Rotary rotary = Rotary(ROTARY_DT, ROTARY_CLK);

Switch encoderButton(ROTARY_BTN, INPUT_PULLUP);

// Counter that will be incremented or decremented by rotation.
int counter = 0;

void setup() {
  Serial.begin(115200);
  Serial.print(F("DCC-EX Rotary Encoder "));
  Serial.println(VERSION);
  Serial.print(F("Available at I2C address 0x"));
  Serial.println(I2C_ADDRESS, HEX);
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {
  encoderButton.poll();

  if (encoderButton.longPress()) {
    // Disable reading position allow rotation to "home"
    encoderRead = false;
    Serial.println(F("Disabling position movement"));
  } else if (encoderButton.singleClick() && encoderRead) {
    // Send rotary position here
  } else if (encoderButton.singleClick() && !encoderRead) {
    // Once rotated to "home", zero counter and enable again
    counter = 0;
    encoderRead = true;
  }

  if (encoderRead) {
    unsigned char result = rotary.process();
    if (result == DIR_CW) {
      counter++;
      Serial.println(counter);
    } else if (result == DIR_CCW) {
      counter--;
      Serial.println(counter);
    }
  }
}

void receiveEvent(int received) {

}

void requestEvent() {

}