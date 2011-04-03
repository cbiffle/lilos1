/*
 * Copyright 2011 Cliff L. Biffle.
 * Released under the Creative Commons Attribution-ShareAlike 3.0 License:
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef LILOS_UTIL_HH_
#define LILOS_UTIL_HH_

/*
 * A few utility macros.
 */

// Use in place of a return type when declaring a function that won't return.
#define NORETURN void __attribute__((noreturn))

// Sometimes it's critical to correctness that a function isn't inlined, or
// is reliably inlined.
#define NEVER_INLINE __attribute__((noinline))
#define ALWAYS_INLINE inline __attribute__((always_inline))

#endif  // LILOS_UTIL_HH_
