/*
 * Copyright 2011 Cliff L. Biffle.
 * Released under the Creative Commons Attribution-ShareAlike 3.0 License:
 * http://creativecommons.org/licenses/by-sa/3.0/
 */


#include <avr/io.h>
#include <stddef.h>

#include <lilos/usart.hh>
#include <lilos/task.hh>

struct UsartCmd {
  enum {
    RAM,
    FLASH,
  } space;
  union {
    const uint8_t *ram_src;
    const prog_char *flash_src;
  };
  size_t len;
};

static const size_t kUsartTaskStackSize = 32;
static uint8_t usartTaskStack[kUsartTaskStackSize];
static NORETURN usartTaskMain() {
  while (1) {
    lilos::Task *sender = lilos::receive();
    const UsartCmd *cmd = (const UsartCmd *) sender->message();
    if (cmd->space == UsartCmd::RAM) {
      for (size_t n = 0; n < cmd->len; n++) {
        UDR0 = cmd->ram_src[n];
        while (!(UCSR0A & (1 << UDRE0))) lilos::yield();
      }
    } else {
      for (size_t n = 0; n < cmd->len; n++) {
        UDR0 = pgm_read_byte(&(cmd->flash_src[n]));
        while (!(UCSR0A & (1 << UDRE0))) lilos::yield();
      }
    }
    lilos::answer(sender, 1);
  }
}

static lilos::Task usartTask(usartTaskMain, usartTaskStack, kUsartTaskStackSize);

void usart_init_raw(uint16_t ubrr) {
  UBRR0H = ubrr >> 8;
  UBRR0L = ubrr;
  // The bootloader doesn't reliably reset this to defaults.
  UCSR0A = 0;
  UCSR0B = (1 << TXEN0);   // Transmitter on.
  UCSR0C = (1 << USBS0)    // 2 stop bits.
         | (3 << UCSZ00);  // 8 data bits.

  lilos::schedule(&usartTask);
}

void usart_send(const uint8_t *src, size_t len) {
  UsartCmd cmd = { UsartCmd::RAM };
  cmd.ram_src = src;
  cmd.len = len;
  lilos::send(&usartTask, (lilos::msg_t) &cmd);
}

void usart_send_P(const prog_char *src, size_t len) {
  UsartCmd cmd = { UsartCmd::FLASH };
  cmd.flash_src = src;
  cmd.len = len;
  lilos::send(&usartTask, (lilos::msg_t) &cmd);
}

void usart_send(uint8_t b) {
  usart_send(&b, 1);
}
