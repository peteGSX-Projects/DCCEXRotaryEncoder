/*
 *  © 2024 Peter Cole
 *
 *  This is the example colour configuration file for the DCC-EX integrated rotary encoder.
 * 
 *  It is highly recommended to copy this to "colours.h" and modify to suit your specific
 *  requirements.
 * 
 *  NOTE: Modifications to this file will be overwritten by future software updates.
 */

#ifndef COLOURS_H
#define COLOURS_H

/////////////////////////////////////////////////////////////////////////////////////
//  Define colour preferences in this file, refer to "Arduino_GFX.h" for the hex values
//  of the existing colours defined with the Arduino GFX library.
/////////////////////////////////////////////////////////////////////////////////////
//  Define any custom colours here using the appropriate hex value
// #define MY_COLOUR 0x1234

/////////////////////////////////////////////////////////////////////////////////////
//  Define the colours to use for each object here
/////////////////////////////////////////////////////////////////////////////////////
#define BACKGROUND_COLOUR BLACK           // Background colour for the display
#define TURNTABLE_COLOUR RED              // Colour of the main turntable body
#define PIT_COLOUR BLUE                   // Colour of the pit line around the display
#define TURNTABLE_HOME_COLOUR LIGHTGREY   // Colour of the home end of the turntable
#define HOME_HIGHLIGHT_COLOUR YELLOW      // Colour of the home end when aligned with a position
#define HOME_COLOUR CYAN                  // Colour of the home position mark around the pit line
#define POSITION_COLOUR MAGENTA           // Colour of each position mark around the pit line
#define POSITION_TEXT_COLOUR GREENYELLOW  // Colour of the position's text description

#endif