/*
 * Copyright 2011 Cliff L. Biffle.
 * Released under the Creative Commons Attribution-ShareAlike 3.0 License:
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef LILOS_PGMSPACE_H_
#define LILOS_PGMSPACE_H_

/*
 * A wrapper around the standard <avr/pgmspace.h> include.
 */

#include <avr/pgmspace.h>

#ifdef __cplusplus
/*
 * Workarounds for an avr-g++ bug that causes spurious warnings.
 */

#undef PROGMEM
#define PROGMEM __attribute__(( section(".progmem.data") ))

#undef PSTR
#define PSTR(s) (__extension__({static prog_char __c[] PROGMEM = (s); &__c[0]; }))

#endif

#endif  // LILOS_PGMSPACE_H_
