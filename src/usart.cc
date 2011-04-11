/*
 * Copyright 2011 Cliff L. Biffle.
 * Released under the Creative Commons Attribution-ShareAlike 3.0 License:
 * http://creativecommons.org/licenses/by-sa/3.0/
 */


#include <avr/io.h>
#include <stddef.h>

#include <lilos/usart.hh>
#include <lilos/task.hh>

namespace lilos {

uint8_t USART::read() {
  ATOMIC {
    if (available()) return readNow();

    return sendVoid(&_receiveTasks);
  }
}

void USART::write(const uint8_t *src, size_t len) {
  for (size_t i = 0; i < len; i++) {
    write(src[i]);
  }
}

void USART::write_P(const prog_char *src, size_t len) {
  for (size_t i = 0; i < len; i++) {
    write(pgm_read_byte(&src[i]));
  }
}

void USART::write(uint8_t b) {
  ATOMIC {
    activateTransmission();
    send(&_transmitTasks, b);
  }
}

}  // namespace lilos
