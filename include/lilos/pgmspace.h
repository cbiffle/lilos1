#ifndef LILOS_PGMSPACE_H_
#define LILOS_PGMSPACE_H_

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
