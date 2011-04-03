/*
 * Copyright 2011 Cliff L. Biffle.
 * Released under the Creative Commons Attribution-ShareAlike 3.0 License:
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include <stddef.h>
#include <stdlib.h>

#include <util/delay.h>
#include <avr/interrupt.h>

#include <lilos/task.hh>
#include <lilos/gpio.hh>
#include <lilos/usart.hh>
#include <lilos/time.hh>
#include <lilos/debug.hh>
#include <lilos/pgmspace.hh>

using lilos::debugWrite;
using lilos::debugWrite_P;
using lilos::debugLn;


/*
 * A trivial message server that controls the debug LED.  Any non-zero message
 * will turn the LED on; zero will turn it off.
 */
TASK(serverTask, 32) {
  static const lilos::port::Pin led = PIN(B, 5);
  led.setDirection(lilos::port::OUT);
  while (1) {
    lilos::Task *sender = lilos::receive();
    led.setValue(sender->message());
    lilos::answer(sender, 0);
  }
}

/*
 * A task that periodically dumps the state of all tasks to the USART.
 */
TASK(debugTask, 80) {
  lilos::IntervalTimer timer(1000);
  while (1) {
    lilos::taskDump();
    timer.wait();
  }
}

/*
 * A task that sends messages turning the debug LED on and off, while announcing
 * its intent on the USART.
 */
TASK(flashTask, 64) {
  lilos::IntervalTimer timer(500);
  while (1) {
    debugWrite_P(PSTR("on\r"));
    lilos::send(&serverTask, true);
    timer.wait();

    debugWrite_P(PSTR("off\r"));
    lilos::send(&serverTask, false);
    timer.wait();
  }
}

NORETURN main() {
  lilos::timeInit();
  lilos::debugInit();
  sei();

  _delay_ms(1000);

  schedule(&debugTask);
  schedule(&flashTask);
  schedule(&serverTask);

  lilos::startTasking();
}
