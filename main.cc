#include <stddef.h>
#include <stdlib.h>

#include <util/delay.h>
#include <avr/interrupt.h>

#include <lilos/task.hh>
#include <lilos/gpio.hh>
#include <lilos/usart.hh>
#include <lilos/time.hh>
#include <lilos/debug.hh>
#include <lilos/pgmspace.h>

using lilos::debugWrite;
using lilos::debugWrite_P;
using lilos::debugLn;

static const lilos::port::Pin led = { lilos::port::B, _BV(5) };

static void debug(uint8_t signal) {
  for (int i = 0; i < 8; i++) {
    led.setValue(signal & 1);
    signal >>= 1;
    _delay_ms(100);
  }
}

TASK(serverTask, 128) {
  while (1) {
    lilos::Task *sender = lilos::receive();
    led.setValue(sender->message());
    lilos::answer(sender, 0);
    lilos::yield();
  }
}

TASK(debugTask, 128) {
  lilos::IntervalTimer timer(1000);
  while (1) {
    lilos::taskDump();
    timer.wait();
  }
}

TASK(flashTask, 128) {
  lilos::IntervalTimer timer(500);
  while (1) {
    debugWrite_P(PSTR("on\r"));
    lilos::msg_t resp = lilos::send(&serverTask, 1);
    timer.wait();
    debugWrite_P(PSTR("off\r"));
    resp = lilos::send(&serverTask, 0);
    timer.wait();
  }
}

int main() {
  lilos::timeInit();
  lilos::debugInit();
  sei();

  led.setDirection(lilos::port::OUT);

  schedule(&debugTask);
  schedule(&flashTask);
  schedule(&serverTask);

//  debugWrite("Starting...\r");
//  lilos::taskDump();

  lilos::startTasking();
}
