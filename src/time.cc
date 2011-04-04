/*
 * Copyright 2011 Cliff L. Biffle.
 * Released under the Creative Commons Attribution-ShareAlike 3.0 License:
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include <avr/io.h>

#include <lilos/atomic.hh>
#include <lilos/time.hh>
#include <lilos/task.hh>
#include <lilos/limits.hh>

namespace lilos {

static uint32_t timerTicks = 0;
static TaskList timerTaskList;

/*
 * The timerTask wakes up once per millisecond (see the ISR at the end of this
 * file) and looks for expired deadlines.  It wakes up any tasks it finds.
 */
TASK(timerTask, 32) {
  Task *me = currentTask();
  while (1) {
    Task *t = me->waiters().head();
    uint32_t time = ticks();
    while (t) {
      Task *next = t->next();  // Cache this in case we answer and change it.
      uint32_t deadline = *t->message<uint32_t *>();
      // Hack: we assume that no deadline will be 2^30 milliseconds from now.
      if (time - deadline < numeric_limits<int32_t>::max / 2) answerVoid(t);
      t = next;
    }
    sendVoid(&timerTaskList);
  }
}

void timeInit() {
  TCCR2A = 2;  // CTC mode
  TCCR2B = 6;  // clk/256
  TIMSK2 = _BV(OCIE2A);  // interrupt on match
  OCR2A = F_CPU / 1000 / 256 - 1;

  timerTaskList.append(&timerTask);
}

uint32_t ticks() {
  ATOMIC {
    return timerTicks;
  }
}

void sleepUntil(uint32_t deadline) {
  sendPtr(&timerTask, &deadline);
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
  // Check to see if the timerTask is waiting for us.  If so, unblock it.
  Task *tt = timerTaskList.headNonAtomic();
  if (tt) answerVoid(tt);
}
