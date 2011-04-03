/*
 * Copyright 2011 Cliff L. Biffle.
 * Released under the Creative Commons Attribution-ShareAlike 3.0 License:
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef LILOS_USART_HH_
#define LILOS_USART_HH_

#include <stddef.h>

#include <lilos/pgmspace.hh>
#include <lilos/static_assert.hh>

namespace lilos {

/*
 * Sets the USART's UBRR clock divider register.  Use this to set the baud rate
 * to values not known at compile time.  (For values that *are* known at
 * compile time, use usart_init, below.)
 */
void usart_init_raw(uint16_t ubrr);

/*
 * Sets the USART's baud rate to a compile-time constant.  This template
 * compiles to a simple constant load and call to usart_init_raw (above).
 *
 * usart_init incorporates static checks to ensure that the specified baud
 * rate is achievable.  It will fail to compile if the baud rate is too high
 * for the chosen clock frequency, or if the effective baud rate differs from
 * the requested rate by more than 5%.
 */
template <uint32_t baudrate>
inline void usart_init() {
  static const uint32_t ubrr32 = F_CPU / 16 / baudrate - 1;
  static const uint32_t actualRate = F_CPU / (16 * (ubrr32 + 1));
  static_assert("Calculated UART rate must fit in 16 bits.",
      (ubrr32 & 0xFFFF0000) == 0);
  static_assert("UART effective baud rate error must be under 5%.",
      (actualRate - baudrate) <= baudrate * 0.05
      && (actualRate - baudrate) >= -(baudrate * 0.05));

  usart_init_raw(ubrr32);
}

/*
 * Receives a character from the USART.  May block.
 */
uint8_t usart_recv();

/*
 * Sends a character to the USART.  May block.
 */
void usart_send(uint8_t);

/*
 * These two functions send blocks of data to the USART.  The blocks are not
 * atomic: if other tasks are trying to transmit at the same time, output may
 * get interleaved.
 */
void usart_send(const uint8_t *, size_t);
void usart_send_P(const prog_char *, size_t);

}  // namespace lilos

#endif  // LILOS_USART_HH_
