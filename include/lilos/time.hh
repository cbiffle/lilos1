#ifndef LILOS_TIME_HH_
#define LILOS_TIME_HH_

/*
 * Routines for dealing with time.
 *
 * This module takes over Timer/Counter 2.
 */

#include <stdint.h>

namespace lilos {

// Start time support.
void timeInit();

// Return the number of microseconds since system start, mod 2^32.
uint32_t ticks();

// Gives up control for the specified number of microseconds.
void sleep(uint32_t count);

}  // namespace lilos

extern "C" void TIMER2_COMPA_vect(void) __attribute__((signal));

#endif  // LILOS_TIME_HH_
