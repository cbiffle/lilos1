#ifndef LILOS_LIMITS_HH_
#define LILOS_LIMITS_HH_

namespace lilos {

template <typename T>
class numeric_limits { };

template <>
class numeric_limits<int32_t> {
public:
  static const int32_t max = 0x7FFFFFFF;
};

}  // namespace lilos

#endif  // LILOS_LIMITS_HH_
