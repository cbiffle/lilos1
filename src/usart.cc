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

static TaskList transmitTasks;
static TaskList receiveTasks;

void usart_init_raw(uint16_t ubrr) {
  UBRR0H = ubrr >> 8;
  UBRR0L = ubrr;
  // The bootloader doesn't reliably reset this to defaults.
  UCSR0A = 0;
  UCSR0B = _BV(TXEN0) | _BV(RXEN0)  // Transmitter and receiver on.
         | _BV(RXCIE0);             // Receive interrupt enabled.
  UCSR0C = (1 << USBS0)    // 2 stop bits.
         | (3 << UCSZ00);  // 8 data bits.

}

void usart_send(const uint8_t *src, size_t len) {
  for (size_t i = 0; i < len; i++) {
    usart_send(src[i]);
  }
}

void usart_send_P(const prog_char *src, size_t len) {
  for (size_t i = 0; i < len; i++) {
    usart_send(pgm_read_byte(&src[i]));
  }
}

void usart_send(uint8_t b) {
  ATOMIC {
    // Enable UDRE interrupt.
    UCSR0B |= _BV(UDRIE0);
    send(&transmitTasks, b);
  }
}

uint8_t usart_recv() {
  ATOMIC {
    if (UCSR0A & _BV(RXC0)) {
      return UDR0;
    }

    return sendVoid(&receiveTasks);
  }
}

}  // namespace lilos

using namespace lilos;

ISR(USART_UDRE_vect) {
  if (!transmitTasks.headNonAtomic()) {
    UCSR0B &= ~_BV(UDRIE0);
  } else {
    Task *sender = transmitTasks.headNonAtomic();
    UDR0 = sender->message();
    answerVoid(sender);
  }
}

ISR(USART_RX_vect) {
  Task *t = receiveTasks.headNonAtomic();
  if (t) answer(t, UDR0);
}
