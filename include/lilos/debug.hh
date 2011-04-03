/*
 * Copyright 2011 Cliff L. Biffle.
 * Released under the Creative Commons Attribution-ShareAlike 3.0 License:
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef LILOS_DEBUG_HH_
#define LILOS_DEBUG_HH_

#include <stdint.h>
#include <lilos/pgmspace.hh>

namespace lilos {

// Start debug system.  Takes over the USART.
void debugInit();

// Writes out a null-terminated string.
void debugWrite(const char *);

// Writes out a null-terminated string from program space.
void debugWrite_P(const prog_char *);

// Writes out a hexadecimal integer.
void debugWrite(uint32_t);

// Ends the line.
void debugLn();

}  // namespace lilos

#endif  // LILOS_DEBUG_HH_
