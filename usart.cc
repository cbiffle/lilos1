
#include <avr/io.h>
#include <stddef.h>

#include "usart.hh"
#include "task.hh"

static volatile uint8_t _txCount = 0;
static volatile const char *_txSource = 0;

void usart_init_raw(uint16_t ubrr) {
  UBRR0H = ubrr >> 8;
  UBRR0L = ubrr;
  // The bootloader doesn't reliably reset this to defaults.
  UCSR0A = 0;
  UCSR0B = (1 << TXEN0);   // Transmitter on.
  UCSR0C = (1 << USBS0)    // 2 stop bits.
         | (3 << UCSZ00);  // 8 data bits.
}

void usart_send(uint8_t b) {
  // Wait until the TX data register is empty.
  while (!(UCSR0A & (1 << UDRE0))) lilos::yield();

  UDR0 = b;
}

void usart_write(const char *src, uint8_t count) {
  if (count == 0) return;

  // Block until previous transmission completes -- this bears rethinking.
  while (_txCount > 0) lilos::yield();

  _txSource = src;
  _txCount = count;

  UCSR0B |= (1 << UDRIE0);  // Enable interrupt on data register empty.
}

void USART_UDRE_vect() {
  uint8_t count = _txCount;
  if (count == 0) return;  // This shouldn't happen.

  UDR0 = *(_txSource++);

  if (count == 1) {
    UCSR0B &= ~(1 << UDRIE0);
  }
  _txCount = count - 1;
}
