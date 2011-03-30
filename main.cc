#include <stddef.h>
#include "task.hh"
#include "AvrPort.hh"
#include <util/delay.h>
#include <stdlib.h>

static const port::Pin led = FASTPIN(B, 5);

static void debug(uint8_t signal) {
  for (int i = 0; i < 8; i++) {
    led.setValue(signal & 1);
    signal >>= 1;
    _delay_ms(100);
  }
}

uint8_t flashStack[128];
void flashMain() {
  while (1) {
    led.setValue(true);
    lilos::yield();
    led.setValue(false);
    lilos::yield();
  }
}

uint8_t delayStack[128];
void delayMain() {
  while (1) {
    _delay_ms(500);
    lilos::yield();
  }
}

static lilos::Task flashTask(flashMain, flashStack, 128);
static lilos::Task delayTask(delayMain, delayStack, 128);

int main() {
  led.setDirection(port::OUT);
  led.setValue(false);
  debug(0xAA);
  led.setValue(false);
  _delay_ms(3000);

  lilos::startTasking();
}
