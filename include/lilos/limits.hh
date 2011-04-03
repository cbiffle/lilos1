#ifndef LILOS_LIMITS_HH_
#define LILOS_LIMITS_HH_

/*
 * Numeric limits, in the style of std::numeric_limits (which is not available
 * on our platform).
 */

#include <stdint.h>  // We rely on __STDC_LIMIT_MACROS being defined!

namespace lilos {

template <typename T>
class numeric_limits { };

template <>
class numeric_limits<int8_t> {
public:
  static const int8_t max = INT8_MAX;
  static const int8_t min = INT8_MIN;
};

template <>
class numeric_limits<uint8_t> {
public:
  static const uint8_t max = UINT8_MAX;
  static const uint8_t min = 0;
};

template <>
class numeric_limits<int16_t> {
public:
  static const int16_t max = INT16_MAX;
  static const int16_t min = INT16_MIN;
};

template <>
class numeric_limits<uint16_t> {
public:
  static const uint16_t max = UINT16_MAX;
  static const uint16_t min = 0;
};

template <>
class numeric_limits<int32_t> {
public:
  static const int32_t max = INT32_MAX;
  static const int32_t min = INT32_MIN;
};

template <>
class numeric_limits<uint32_t> {
public:
  static const uint32_t max = UINT32_MAX;
  static const uint32_t min = 0;
};

}  // namespace lilos

#endif  // LILOS_LIMITS_HH_
