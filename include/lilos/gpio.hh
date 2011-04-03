/*
 * Copyright 2011 Cliff L. Biffle.
 * Released under the Creative Commons Attribution-ShareAlike 3.0 License:
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef LILOS_GPIO_HH_
#define LILOS_GPIO_HH_

/*
 * Describes the AVR's GPIO pins.  The classes and functions here are carefully
 * designed to compile away for the most common use cases.  For example, a 
 * sequence like this:
 *
 *  static const port::Pin led = PIN(B, 5);
 *
 *  void turnOn() {
 *    led.setValue(true);
 *  }
 *
 * This compiles to a single sbi instruction, and the led definition is compiled
 * away, saving RAM.
 *
 * This file uses ALWAYS_INLINE aggressively, because gcc at -Os tends not to
 * inline these functions...which greatly increases code size!
 */

#include <avr/io.h>

namespace lilos {

namespace port {

typedef uint8_t port_base_t;

enum Direction { IN = 0, OUT = 1 };

ALWAYS_INLINE volatile uint8_t &ioreg(uint32_t base, int32_t disp = 0) {
  return _SFR_IO8(base + disp);
}

class Pin {
public:
  // The base of the port's IO registers (the IO-space address of PINx).
  port_base_t port_base;
  // The pin mask, one-hot encoded.
  uint8_t pin_mask;

  ALWAYS_INLINE bool getValue() const {
    volatile uint8_t &pin = ioreg(port_base, 0);
    return !!(pin & pin_mask);
  }

  ALWAYS_INLINE void setDirection(Direction d) const {
    volatile uint8_t &ddr = ioreg(port_base, 1);
    if (d == IN) {
      ddr &= ~pin_mask;
    } else {
      ddr |=  pin_mask;
    }
  }

  ALWAYS_INLINE void setValue(bool on) const {
    volatile uint8_t &port = ioreg(port_base, 2);
    if (!on) {
      port &= ~pin_mask;
    } else {
      port |=  pin_mask;
    }
  }
};

class Port {
public:
  port_base_t port_base;

  ALWAYS_INLINE Pin pin(uint8_t pin_number) const {
    return (Pin) { port_base, 1 << pin_number };
  }
};

// Port base addresses.
enum {
  A = 0,
  B = 3,
  C = 6,
  D = 9,
};

#define PIN(P,N) { lilos::port::P, _BV(N) }

};  // namespace port

}  // namespace lilos

#endif // LILOS_GPIO_HH_
