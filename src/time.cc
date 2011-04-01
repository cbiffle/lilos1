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

void sleepUntil(uint32_t deadline) {
  while (deadline < ticks()) yield();
  while (deadline > ticks()) yield();
}

IntervalTimer::IntervalTimer(uint16_t interval)
: _deadline(ticks() + interval),
  _interval(interval) {}

void IntervalTimer::wait() {
  sleepUntil(_deadline);
  _deadline += _interval;
}

}  // namespace lilos

void TIMER2_COMPA_vect() {
  lilos::timerTicks++;
}
