#include <avr/io.h>
#include <util/atomic.h>

#include "time.hh"

namespace lilos {

static uint16_t timerTopBits = 0;

void timeInit() {
  timerTopBits = 0;
  TCCR1A = 0;  // normal mode
  TCCR1B = 2;  // clk/8
  TIMSK1 = _BV(TOIE1);  // interrupt on overflow
}

uint32_t ticks() {
  union {
    struct {
      uint16_t low;
      uint16_t high;
    } parts;
    uint32_t assembled;
  };

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    parts.low = TCNT1;
    parts.high = timerTopBits;
  }

  return assembled;
}

}  // namespace lilos

void TIMER1_OVF_vect() {
  lilos::timerTopBits++;
}
