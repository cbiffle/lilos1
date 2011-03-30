#include <stddef.h>
#include "task.hh"
#include "AvrPort.hh"
#include <util/delay.h>
#include <stdlib.h>
#include "usart.hh"
#include <avr/interrupt.h>
#include "time.hh"

static const port::Pin led = FASTPIN(B, 5);

static void debug(uint8_t signal) {
  for (int i = 0; i < 8; i++) {
    led.setValue(signal & 1);
    signal >>= 1;
    _delay_ms(100);
  }
}

static void send_hex(uint32_t val) {
  for (int i = 0; i < 8; i++) {
    uint8_t nibble = (val >> 28) & 0xF;
    if (nibble > 9) {
      nibble = (nibble - 10) + 'A';
    } else {
      nibble += '0';
    }
    usart_send(nibble);
    val <<= 4;
  }
  usart_send((uint8_t) ' ');
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

uint8_t txStack[128];
void txMain() {
  while (1) {
    send_hex(lilos::ticks());
    lilos::yield();
  }
}

static lilos::Task flashTask(flashMain, flashStack, 128);
static lilos::Task delayTask(delayMain, delayStack, 128);
static lilos::Task txTask(txMain, txStack, 128);

int main() {
  usart_init<38400>();
  lilos::timeInit();
  sei();

  led.setDirection(port::OUT);
  led.setValue(false);
  debug(0xAA);
  led.setValue(false);
  _delay_ms(3000);

  flashTask.schedule();
  delayTask.schedule();
  txTask.schedule();

  lilos::startTasking();
}
