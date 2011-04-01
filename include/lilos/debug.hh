#ifndef LILOS_DEBUG_HH_
#define LILOS_DEBUG_HH_

#include <stdint.h>
#include <avr/pgmspace.h>

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
