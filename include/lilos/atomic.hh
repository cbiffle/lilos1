#ifndef LILOS_ATOMIC_HH_
#define LILOS_ATOMIC_HH_

#include <util/atomic.h>

#define ATOMIC ATOMIC_BLOCK(ATOMIC_RESTORESTATE)

#endif  // LILOS_ATOMIC_HH_
