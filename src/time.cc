#include <avr/io.h>
#include <util/atomic.h>

#include <lilos/time.hh>
#include <lilos/task.hh>

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

void usleep(uint32_t count) {
  uint32_t deadline = ticks() + count;
  while (deadline < ticks()) yield();
  while (deadline > ticks()) yield();
}

}  // namespace lilos

void TIMER1_OVF_vect() {
  lilos::timerTopBits++;
}
