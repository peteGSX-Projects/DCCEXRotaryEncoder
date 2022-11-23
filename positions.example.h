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

#define HOME_ANGLE 0

#define NUMBER_OF_POSITIONS 10

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