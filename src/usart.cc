
#include <avr/io.h>
#include <stddef.h>

#include <lilos/usart.hh>
#include <lilos/task.hh>

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
  UDR0 = b;
  while (!(UCSR0A & (1 << UDRE0))) lilos::yield();
}
