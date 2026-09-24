#ifndef YADAW_SRC_UTIL_TUPLEUTIL
#define YADAW_SRC_UTIL_TUPLEUTIL

#include <tuple>
#include <type_traits>
#include <utility>

template<template<typename> typename Pred, typename... T>
using FilteredTuple = decltype(
    std::tuple_cat(
        std::declval<std::conditional_t<Pred<T>::Value, std::tuple<T>, std::tuple<>>>()...
    )
);

template <template <typename> typename Component, typename Tuple>
struct TransformedTuple;

template <template <typename> typename Component, typename... Ts>
struct TransformedTuple<Component, std::tuple<Ts...>>
{
    using Type = std::tuple<Component<Ts>...>;
};

#endif // YADAW_SRC_UTIL_TUPLEUTIL
