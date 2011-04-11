/*
 * Copyright 2011 Cliff L. Biffle.
 * Released under the Creative Commons Attribution-ShareAlike 3.0 License:
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include <string.h>
#include <lilos/debug.hh>
#include <lilos/usart.hh>
#include <avr/io.h>
#include <lilos/board_debug.hh>

namespace lilos {

void debugInit() {
  debugUsart.initialize(kDebugBaudrate,
                        USART::DATA_8, USART::PARITY_NONE, USART::STOP_1);
}

void debugWrite(const char *str) {
  size_t len = strlen(str);
  debugUsart.write((const uint8_t *) str, len);
}

void debugWrite_P(const prog_char *str) {
  size_t len = strlen_P(str);
  debugUsart.write_P(str, len);
}

void debugWrite(uint32_t word) {
  char buf[9];
  for (int i = 0; i < 8; i++) {
    uint8_t nibble = (word >> 28) & 0xF;
    char out;
    if (nibble < 10) {
      out = nibble + '0';
    } else {
      out = (nibble - 10) + 'A';
    }
    buf[i] = out;
    word <<= 4;
  }
  buf[8] = ' ';
  debugUsart.write((uint8_t *) buf, 9);
}

void debugLn() {
  debugUsart.write('\r');
}

}  // namespace lilos
