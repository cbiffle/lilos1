/*
 * Copyright 2011 Cliff L. Biffle.
 * Released under the Creative Commons Attribution-ShareAlike 3.0 License:
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef __STATIC_ASSERT_HH__
#define __STATIC_ASSERT_HH__

/*
 * A macro for checking conditions at compile time.
 * Example:
 *  static_assert(x > 10);
 *
 * Unfortunately the error reporting is a little poor.
 */

/*
 * Helper class, named to make the error reporting somewhat
 * coherent.
 */
template <bool> class StaticAssertFailed {};
template <> class StaticAssertFailed<false> {
private:
  StaticAssertFailed();
};


#define static_assert(msg, c) StaticAssertFailed<(c)>()

#endif  // __STATIC_ASSERT_HH__
