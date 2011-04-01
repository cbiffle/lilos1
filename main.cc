#include <stddef.h>
#include <stdlib.h>

#include <util/delay.h>
#include <avr/interrupt.h>

#include <lilos/task.hh>
#include <lilos/gpio.hh>
#include <lilos/usart.hh>
#include <lilos/time.hh>
#include <lilos/debug.hh>

static const lilos::port::Pin led = { lilos::port::B, _BV(5) };

static void debug(uint8_t signal) {
  for (int i = 0; i < 8; i++) {
    led.setValue(signal & 1);
    signal >>= 1;
    _delay_ms(100);
  }
}

uint8_t debugStack[128];
void debugMain() {
  lilos::IntervalTimer timer(1000);
  while (1) {
    lilos::taskDump();
    timer.wait();
  }
}

uint8_t onStack[128];
void onMain() {
  lilos::IntervalTimer timer(1000);
  while (1) {
    lilos::debugWrite("on\r");
    led.setValue(true);
    timer.wait();
  }
}

uint8_t offStack[128];
void offMain() {
  lilos::IntervalTimer timer(500);
  while (1) {
    timer.wait();
    lilos::debugWrite("off\r");
    led.setValue(false);
    timer.wait();
  }
}

static lilos::Task onTask(onMain, onStack, 128);
static lilos::Task offTask(offMain, offStack, 128);
static lilos::Task debugTask(debugMain, debugStack, 128);

int main() {
  lilos::timeInit();
  lilos::debugInit();
  sei();

  led.setDirection(lilos::port::OUT);
  led.setValue(false);
  debug(0xAA);
  led.setValue(false);
  _delay_ms(3000);

  onTask.schedule();
  offTask.schedule();
  debugTask.schedule();

  lilos::debugWrite("Starting...\r");
  lilos::taskDump();

  lilos::startTasking();
}
