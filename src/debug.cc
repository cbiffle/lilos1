#include <lilos/debug.hh>
#include <lilos/usart.hh>
#include <avr/io.h>

namespace lilos {

void debugInit() {
  usart_init<38400>();
}

void debugWrite(const char *str) {
  while (char c = *str++) {
    usart_send(c);
  }
}

void debugWrite(uint32_t word) {
  for (int i = 0; i < 8; i++) {
    uint8_t nibble = (word >> 28) & 0xF;
    char out;
    if (nibble < 10) {
      out = nibble + '0';
    } else {
      out = (nibble - 10) + 'A';
    }
    usart_send(out);
  }
  usart_send(' ');
}

void debugLn() {
  usart_send('\r');
}

}  // namespace lilos
