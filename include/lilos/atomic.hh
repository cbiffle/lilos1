/*
 * Copyright 2011 Cliff L. Biffle.
 * Released under the Creative Commons Attribution-ShareAlike 3.0 License:
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef LILOS_ATOMIC_HH_
#define LILOS_ATOMIC_HH_

/*
 * An alternative to the official <util/atomic.h> header.  Currently it just
 * provides shorthand.
 */

#include <util/atomic.h>

/*
 * Disables interrupts and restores the previous state (disabled or not) at
 * completion.  Slightly more expensive than a straight cli/sei pair, but more
 * composable: your code need not care whether it was called with interrupts
 * enabled.
 *
 * Example of use:
 *  ATOMIC {
 *    do_work();
 *  }
 */
#define ATOMIC ATOMIC_BLOCK(ATOMIC_RESTORESTATE)

#endif  // LILOS_ATOMIC_HH_
