#ifndef LILOS_TIME_HH_
#define LILOS_TIME_HH_

/*
 * Routines for dealing with time.
 *
 * This module takes over Timer/Counter 1.
 */

#include <stdint.h>

namespace lilos {

// Start time support.
void timeInit();

// Return the number of microseconds since system start, mod 2^32.
uint32_t ticks();

}  // namespace lilos

extern "C" void TIMER1_OVF_vect(void) __attribute__((signal));

#endif  // LILOS_TIME_HH_
