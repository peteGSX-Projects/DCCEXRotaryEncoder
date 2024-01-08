/*
 *  © 2024 Peter Cole
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

#ifndef DEFINES_H
#define DEFINES_H

#include <Arduino.h>

/*
Create a struct to define turntable positions (defined in positions.h).
*/
typedef struct {
  uint16_t angle;
  uint8_t positionId;
  char description[11];
} positionDefinition;

enum {
  RE_RDY = 0xA0,    // Flag to device driver that encoder is ready
  RE_VER = 0xA1,    // Flag to send version to device driver
  RE_READ = 0xA2,   // Flag the device driver is requesting the current position
  RE_OP = 0xA3,     // Flag for operationg start/end, received for feedback on move start/end
  RE_MOVE = 0xA4,   // Flag device driver is sending a position to move to
  RE_ERR = 0xAF,    // Flag device driver has asked for something unknown
};

#endif
