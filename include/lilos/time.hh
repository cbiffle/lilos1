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

// Return the number of milliseconds since system start, mod 2^32.
uint32_t ticks();

// Waits for a particular time to pass.  To sleep for n milliseconds, use:
//  sleepUntil(ticks() + time)
void sleepUntil(uint32_t time);

class IntervalTimer {
  uint32_t _deadline;
  uint16_t _interval;

public:
  IntervalTimer(uint16_t interval);

  void wait();
};

}  // namespace lilos

extern "C" void TIMER2_COMPA_vect(void) __attribute__((signal));

#endif  // LILOS_TIME_HH_
