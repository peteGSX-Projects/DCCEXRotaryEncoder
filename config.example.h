/*
 *  © 2022 Peter Cole
 *
 *  This is the example configuration file for the DCC-EX integrated rotary encoder.
 * 
 *  It is highly recommended to copy this to "config.h" and modify to suit your specific
 *  requirements.
 * 
 *  NOTE: Modifications to this file will be overwritten by future software updates.
 */
#ifndef CONFIG_H
#define CONFIG_H

/////////////////////////////////////////////////////////////////////////////////////
//  START: General configuration options.
/////////////////////////////////////////////////////////////////////////////////////
#define I2C_ADDRESS 0x80  // Default 0x80, can be any valid, available I2C address
#define MODE TURNTABLE    // Default TURNTABLE
// #define MODE KNOB
// #define DIAG           // Uncomment to enable continous output of encoder position
#define FEEDBACK          // Comment out to disable feedback from CommandStation
/////////////////////////////////////////////////////////////////////////////////////
//  END: General configuration options.
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//  START: Rotary encoder configuration options.
/////////////////////////////////////////////////////////////////////////////////////
#define HALF_STEP         // Comment out to use full step mode
#define POLARITY 0        // Set to 1 to reverse rotation direction
#define DEBOUNCE 50       // Adjust if necessary to prevent false button presses
#define LONG_PRESS 1000   // Adjust if necessary for long press detection
#define ENABLE_PULLUPS    // Comment out if input does not require pull up
#define ROTARY_BTN 2      // Define encoder button pin
#define ROTARY_DT 5       // Define encoder DT pin
#define ROTARY_CLK 6      // Define encoder clock pin

// Values returned by 'process', these should not need modification.
// No complete step yet.
#define DIR_NONE 0x0
// Clockwise step.
#define DIR_CW 0x10
// Anti-clockwise step.
#define DIR_CCW 0x20
/////////////////////////////////////////////////////////////////////////////////////
//  END: Rotary encoder configuration options.
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//  START: KNOB mode configuration options.
/////////////////////////////////////////////////////////////////////////////////////
#define OLED_CS  7        // Define OLED CS/chip select pin
#define OLED_DC  9        // Define OLED DC pin
/////////////////////////////////////////////////////////////////////////////////////
//  END: KNOB mode configuration options.
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//  START: TURNTABLE mode configuration options.
/////////////////////////////////////////////////////////////////////////////////////
#define GC9A01_DIN 11     // Define GC9A01 DIN pin
#define GC9A01_CLK 13     // Define GC9A01 CLK/clock pin
#define GC9A01_CS 10      // Define GC9A01 CS/chip select pin
#define GC9A01_DC 7       // Define GC9A01 DC pin
#define GC9A01_RST 8      // Define GC9A01 RST/reset pin
#define GC9A01_BL 9       // Define GC9A01 BL/backlight pin
// Rotation can be 0, 1 (90 degrees), 2 (180 degrees), or 3 (270 degrees)
// Rotates entire display
#define GC9A01_ROTATION 0
// Needs to be set true in order to correctly display colours
#define GC9A01_IPS true
//  Number of pixels to inset the representation of the turntable pit.
#define PIT_OFFSET 30
/////////////////////////////////////////////////////////////////////////////////////
//  END: TURNTABLE mode configuration options.
/////////////////////////////////////////////////////////////////////////////////////

#endif