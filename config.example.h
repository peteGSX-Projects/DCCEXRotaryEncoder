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
#define I2C_ADDRESS 0x70  // Default 0x70, can be any valid, available I2C address
#define MODE TURNTABLE    // Default TURNTABLE
// #define MODE KNOB
// #define DIAG           // Uncomment to enable continous output of encoder position
#define BLINK_RATE 500    // Delay in ms to blink text when moving
#if defined(ARDUINO_ARCH_ESP32)
#define I2C_SDA 21        // SDA pin - required for ESP32 only
#define I2C_SCL 22        // SCL pin - required for ESP32 only
#endif
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
#if defined(ARDUINO_ARCH_ESP32)
#define ROTARY_BTN 33      // Define encoder button pin
#define ROTARY_DT 25       // Define encoder DT pin
#define ROTARY_CLK 26      // Define encoder clock pin
#elif defined(ARDUINO_BLACKPILL_F411CE)
#define ROTARY_BTN PB15      // Define encoder button pin
#define ROTARY_DT PB14       // Define encoder DT pin
#define ROTARY_CLK PB13      // Define encoder clock pin
#else
#define ROTARY_BTN 2      // Define encoder button pin
#define ROTARY_DT 5       // Define encoder DT pin
#define ROTARY_CLK 6      // Define encoder clock pin
#endif

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
//  GC9A01 wire colours:
//  VCC Purple
//  GND White
//  DIN Green
//  CLK Orange
//  CS  Yellow
//  DC  Blue
//  RST Brown
//  BL  Grey
/////////////////////////////////////////////////////////////////////////////////////
#if defined(ARDUINO_ARCH_ESP32)
#define GC9A01_DIN 23     // Define GC9A01 DIN pin
#define GC9A01_CLK 18     // Define GC9A01 CLK/clock pin
#define GC9A01_CS 5       // Define GC9A01 CS/chip select pin
#define GC9A01_DC 27      // Define GC9A01 DC pin
#define GC9A01_RST 4      // Define GC9A01 RST/reset pin
#define GC9A01_BL 15      // Define GC9A01 BL/backlight pin
#elif defined(ARDUINO_BLACKPILL_F411CE)
#define GC9A01_DIN PA7     // Define GC9A01 DIN pin
#define GC9A01_CLK PA5     // Define GC9A01 CLK/clock pin
#define GC9A01_CS PA4       // Define GC9A01 CS/chip select pin
#define GC9A01_DC PA3      // Define GC9A01 DC pin
#define GC9A01_RST PA2      // Define GC9A01 RST/reset pin
#define GC9A01_BL PA1      // Define GC9A01 BL/backlight pin
#else
#define GC9A01_DIN 11     // Define GC9A01 DIN pin
#define GC9A01_CLK 13     // Define GC9A01 CLK/clock pin
#define GC9A01_CS 10      // Define GC9A01 CS/chip select pin
#define GC9A01_DC 7       // Define GC9A01 DC pin
#define GC9A01_RST 8      // Define GC9A01 RST/reset pin
#define GC9A01_BL 9       // Define GC9A01 BL/backlight pin
#endif
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