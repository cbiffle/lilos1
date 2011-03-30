/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef SHARED_AVR_PORT_HH_
#define SHARED_AVR_PORT_HH_

#include <avr/io.h>

namespace port {

typedef uint8_t port_base_t;

// By default we compile at -Os, which makes the inlining heuristics more
// conservative...and actually makes the code larger.  This attribute lets
// us subvert the compiler.
#define ALWAYS_INLINE inline __attribute__((always_inline))

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

#define FASTPIN(P,N) ((port::Pin) { port::P, _BV(N) })

};  // namespace port

#endif // SHARED_AVR_PORT_HH_

