#include <stddef.h>
#include "task.hh"
#include "AvrPort.hh"
#include <util/delay.h>
#include <stdlib.h>

static const port::Pin led = FASTPIN(B, 5);

static lilos::task_t flashTask, delayTask;

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
    lilos::yieldTo(&delayTask);
    led.setValue(false);
    lilos::yieldTo(&delayTask);
  }
}

uint8_t delayStack[128];
void delayMain() {
  while (1) {
    _delay_ms(500);
    lilos::yieldTo(&flashTask);
  }
}

int main() {
  led.setDirection(port::OUT);
  led.setValue(false);
  debug(0xAA);
  _delay_ms(3000);

  flashTask = lilos::initTask(flashMain, flashStack, 128);
  delayTask = lilos::initTask(delayMain, delayStack, 128);

  lilos::startTasking(&flashTask);
}
