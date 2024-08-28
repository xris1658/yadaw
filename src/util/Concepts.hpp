#ifndef YADAW_SRC_UTIL_CONCEPTS
#define YADAW_SRC_UTIL_CONCEPTS

#include <concepts>

template<typename Base, typename Derived>
concept DerivedTo = std::derived_from<Base, Derived>;

#endif // YADAW_SRC_UTIL_CONCEPTS