// Version must be numeric only to be able to be sent to the device driver
#define VERSION "0.0.6"

// 0.0.6 includes:
//  - Added support for receiving a position update from EXRAIL routes
// 0.0.5 includes:
//  - Add ESP32 Dev Kit support
// 0.0.4 includes:
//  - Bug fix where position is updated in turntable controller mode
//    regardless if it is aligned with a position or not.
// 0.0.3 includes:
//  - Add boolean feedback via second VPin (1 moving, 0 complete).
//  - Add sending rotary encoder version to device driver.
// 0.0.2-Alpha includes:
//  - Add support for GC9A01 round colour LCD display.
//  - Includes displaying rotating turntable via rotary encoder.
// 0.0.1-Alpha includes:
//  - Initial Alpha testing release.
//  - Select position with rotary encoder to send to CommandStation.
//  - Disply selected position on monochrome SPI OLED.