#ifndef __UART_HH__
#define __UART_HH__

#include "static_assert.hh"

void usart_init_raw(uint16_t ubrr);

template <uint32_t baudrate>
inline void usart_init() {
  static const uint32_t ubrr32 = F_CPU / 16 / baudrate - 1;
  static const uint32_t actualRate = F_CPU / (16 * (ubrr32 + 1));
  static_assert("Calculated UART rate must fit in 16 bits.",
      (ubrr32 & 0xFFFF0000) == 0);
  static_assert("UART effective baud rate error must be under 5%.",
      (actualRate - baudrate) < baudrate * 0.05
      && (actualRate - baudrate) > -(baudrate * 0.05));

  usart_init_raw(ubrr32);
}

void usart_send(uint8_t);
void usart_write(const char *, uint8_t);

extern "C" void USART_UDRE_vect(void) __attribute__((signal));

#endif  // __UART_HH__
