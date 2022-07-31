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
Switch encoderButton(ROTARY_BTN, INPUT_PULLUP);

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

void setup() {
  Serial.begin(115200);
  Serial.print(F("DCC-EX Rotary Encoder "));
  Serial.println(VERSION);
  Serial.print(F("Available at I2C address 0x"));
  Serial.println(I2C_ADDRESS, HEX);
  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(requestEvent);
}

void loop() {
  encoderButton.poll();

  if (encoderButton.longPress()) {
    // Disable reading position allow rotation to "home"
    encoderRead = false;
    Serial.println(F("Disabling position counts"));
  } else if (encoderButton.singleClick() && encoderRead) {
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
    if (result == DIR_CW) {
      if (counter < 127) {
        counter++;
        Serial.println(counter);
      }
    } else if (result == DIR_CCW) {
      if (counter > -127) {
        counter--;
        Serial.println(counter);
      }
    }
  }
}

void requestEvent() {
  Wire.write(position);
}