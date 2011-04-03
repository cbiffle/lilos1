/*
 * Copyright 2011 Cliff L. Biffle.
 * Released under the Creative Commons Attribution-ShareAlike 3.0 License:
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef __UART_HH__
#define __UART_HH__

#include <stddef.h>
#include <lilos/pgmspace.hh>
#include <lilos/static_assert.hh>

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

uint8_t usart_recv();
void usart_send(uint8_t);
void usart_send(const uint8_t *, size_t);
void usart_send_P(const prog_char *, size_t);

#endif  // __UART_HH__
