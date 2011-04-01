#include <string.h>
#include <lilos/debug.hh>
#include <lilos/usart.hh>
#include <avr/io.h>

namespace lilos {

void debugInit() {
  usart_init<38400>();
}

void debugWrite(const char *str) {
  size_t len = strlen(str);
  usart_send((const uint8_t *) str, len);
}

void debugWrite_P(const prog_char *str) {
  size_t len = strlen_P(str);
  usart_send_P(str, len);
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
  usart_send((uint8_t *) buf, 9);
}

void debugLn() {
  usart_send('\r');
}

}  // namespace lilos
