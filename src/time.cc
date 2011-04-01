#include <avr/io.h>
#include <util/atomic.h>

#include <lilos/time.hh>
#include <lilos/task.hh>

namespace lilos {

static uint32_t timerTicks = 0;

void timeInit() {
  TCCR2A = 2;  // CTC mode
  TCCR2B = 6;  // clk/256
  TIMSK2 = _BV(OCIE2A);  // interrupt on match
  OCR2A = F_CPU / 1000 / 256 - 1;
}

uint32_t ticks() {
  return timerTicks;
}

void sleep(uint32_t count) {
  uint32_t deadline = ticks() + count;
  while (deadline < ticks()) yield();
  while (deadline > ticks()) yield();
}

}  // namespace lilos

void TIMER2_COMPA_vect() {
  lilos::timerTicks++;
}
