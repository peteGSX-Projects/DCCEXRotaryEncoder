/*
 *  © 2022 Peter Cole
 *
 *  This is the example position definition file for the DCC-EX integrated rotary encoder.
 * 
 *  It is highly recommended to copy this to "positions.h" and modify to suit your specific
 *  requirements.
 * 
 *  NOTE: Modifications to this file will be overwritten by future software updates.
 */

#ifndef POSITIONS_H
#define POSITIONS_H

/////////////////////////////////////////////////////////////////////////////////////
//  NOTE: All angles are expressed in degrees from the top of the display, or in other
//  words 12 o'clock on an analogue clock.
/////////////////////////////////////////////////////////////////////////////////////
#define HOME_ANGLE 0            // Define the angle in degrees for the home position mark
#define NUMBER_OF_POSITIONS 10  // Define the number of positions to be defined

/////////////////////////////////////////////////////////////////////////////////////
//  You must define the exact number of positions as defined for NUMBER_OF_POSITIONS.
//  The syntax must be correct for each line, and there must be no comma "," at the end
//  of the last defined position.
//  The syntax is:
/*
positionDefinition turntablePositions[NUMBER_OF_POSITIONS] = {
  {angle, id, "description"},
  {angle, id, "description"},
  ...
  {angle, id, "description"}
};
*/
//  Where:
//  angle - Specified in degrees from the top of the display (12 o'clock)
//  id - The identifier that will be sent to the CommandStation when selected (1 - 255)
//  "description" - A 10 character or less position description to display when selected
/////////////////////////////////////////////////////////////////////////////////////

positionDefinition turntablePositions[NUMBER_OF_POSITIONS] = {
  {5, 1, "Test 1"},
  {10, 2, "Test 2"},
  {45, 3, "Test 3"},
  {80, 4, "Test 4"},
  {90, 5, "Test 5"},
  {150, 6, "Test 6"},
  {220, 7, "Test 7"},
  {300, 8, "Test 8"},
  {325, 9, "Test 9"},
  {350, 10, "Test 10"}
};

#endif