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

/////////////////////////////////////////////////////////////////////////////////////
//  Define a valid (and free) I2C address, 0x80 is the default.
// 
#define I2C_ADDRESS 0x80

/////////////////////////////////////////////////////////////////////////////////////
//  .
// 
#define HALF_STEP

/////////////////////////////////////////////////////////////////////////////////////
//  .
// 
#define ROTARY_BTN 2
#define ROTARY_DT 5
#define ROTARY_CLK 6

/////////////////////////////////////////////////////////////////////////////////////
//  .
//
#define POLARITY 0
#define DEBOUNCE 50
#define LONG_PRESS 1000

/////////////////////////////////////////////////////////////////////////////////////
//  .
// Enable weak pullups
#define ENABLE_PULLUPS

// Values returned by 'process'
// No complete step yet.
#define DIR_NONE 0x0
// Clockwise step.
#define DIR_CW 0x10
// Anti-clockwise step.
#define DIR_CCW 0x20

/////////////////////////////////////////////////////////////////////////////////////
//  Define the display to use.
//  USE_OLED - Use a standard SSD1306 (or compatible) SPI attached monochrome OLED
//  USE_GC9A01 - Use a round GC9A01 SPI attached colour LCD
// 
// #define USE_OLED
#define USE_GC9A01

/////////////////////////////////////////////////////////////////////////////////////
//  OLED.
// 
#define OLED_CS  7
#define OLED_DC  9




/////////////////////////////////////////////////////////////////////////////////////
//  .
//
// #define DIAG



/////////////////////////////////////////////////////////////////////////////////////
//  GC9A01 specific defines below.
//
#define GC9A01_DIN 11
#define GC9A01_CLK 13
#define GC9A01_CS 10
#define GC9A01_DC 7
#define GC9A01_RST 8
#define GC9A01_BL 9
#define GC9A01_ROTATION 0
#define GC9A01_IPS true

/////////////////////////////////////////////////////////////////////////////////////
//  Number of pixels to inset the representation of the turntable pit.
//
#define PIT_OFFSET 30