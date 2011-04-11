/*
 * Copyright 2011 Cliff L. Biffle.
 * Released under the Creative Commons Attribution-ShareAlike 3.0 License:
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include <lilos/usart.hh>
#include <lilos/mcu_usart.hh>

namespace lilos {

/*
 * Because the 328P has only one USART, this module doesn't bother defining
 * USARTRegisters.
 */

USART usart0(0);

void USART::initialize(uint32_t baudrate, DataBits db, Parity p, StopBits sb) {
  ATOMIC {
    UBRR0 = (uint16_t) (F_CPU / 16 / baudrate - 1);

    UCSR0A = 0;

    uint8_t b = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
    switch (db) {
      case DATA_9: b |= _BV(UCSZ02); break;
      default: break;
    }
    UCSR0B = b;

    uint8_t parityFlags;
    switch (p) {
      case PARITY_NONE: parityFlags = 0b00; break;
      case PARITY_EVEN: parityFlags = 0b10; break;
      case PARITY_ODD:  parityFlags = 0b11; break;
    }

    uint8_t stopBitFlags;
    switch (sb) {
      case STOP_1: stopBitFlags = 0; break;
      case STOP_2: stopBitFlags = 1; break;
    }

    uint8_t dataBitsFlags;
    switch (db) {
      case DATA_5: dataBitsFlags = 0b00; break;
      case DATA_6: dataBitsFlags = 0b01; break;
      case DATA_7: dataBitsFlags = 0b10; break;
      case DATA_8: dataBitsFlags = 0b11; break;
      case DATA_9: dataBitsFlags = 0b11; break;
    }

    UCSR0C = (parityFlags << UPM00)
           | (stopBitFlags << USBS0)
           | (dataBitsFlags << UCSZ00);
  }
}

uint8_t USART::readNow() {
  return UDR0;
}

void USART::activateTransmission() {
  UCSR0B |= _BV(UDRIE0);
}

bool USART::available() {
  return UCSR0A & _BV(RXC0);
}

}  // namespace lilos

using namespace lilos;

ISR(USART_UDRE_vect) {
  Task *sender = usart0.transmitTasks().head();
  if (!sender) {
    UCSR0B &= ~_BV(UDRIE0);
  } else {
    UDR0 = sender->message();
    answerVoid(sender);
  }
}

ISR(USART_RX_vect) {
  Task *t = usart0.receiveTasks().head();
  if (t) answer(t, UDR0);
}
