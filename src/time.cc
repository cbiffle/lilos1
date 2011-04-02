#include <avr/io.h>

#include <lilos/atomic.hh>
#include <lilos/time.hh>
#include <lilos/task.hh>
#include <lilos/limits.hh>

namespace lilos {

static uint32_t timerTicks = 0;
static TaskList timerTaskList;

TASK(timerTask, 64) {
  Task *me = currentTask();
  while (1) {
    Task *t = me->waiters().head();
    uint32_t time = ticks();
    while (t) {
      Task *next = t->next();
      uint32_t deadline = *reinterpret_cast<uint32_t *>(t->message());
      if (time - deadline < numeric_limits<int32_t>::max / 2) answer(t, 0);
      t = next;
    }
    send(&timerTaskList, 0);
  }
}

void timeInit() {
  TCCR2A = 2;  // CTC mode
  TCCR2B = 6;  // clk/256
  TIMSK2 = _BV(OCIE2A);  // interrupt on match
  OCR2A = F_CPU / 1000 / 256 - 1;

  timerTaskList.appendAtomic(&timerTask);
}

uint32_t ticks() {
  ATOMIC {
    return timerTicks;
  }
}

void sleepUntil(uint32_t deadline) {
  send(&timerTask, (msg_t) &deadline);
}

IntervalTimer::IntervalTimer(uint16_t interval)
: _deadline(ticks() + interval),
  _interval(interval) {}

void IntervalTimer::wait() {
  sleepUntil(_deadline);
  _deadline += _interval;
}

}  // namespace lilos

using namespace lilos;

void TIMER2_COMPA_vect() {
  timerTicks++;
  Task *tt = timerTaskList.headNonAtomic();
  if (tt) answer(tt, 0);
}
