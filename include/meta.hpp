#ifndef ELDERLISP_META_HPP
#define ELDERLISP_META_HPP

#include <cstddef>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

namespace el {

template<std::size_t Index, class T>
struct TypeIndex {
    template<class U>
    requires std::is_same_v<T, U>
    static auto constexpr value() -> std::size_t
    {
        return Index;
    }

    template<size_t I>
    requires(I == Index) static auto constexpr type() -> T;
};

template<class IndexSequence, class... TypeList>
struct TypeIndexer_impl;

template<std::size_t... Indices, class... TypeList>
struct TypeIndexer_impl<std::index_sequence<Indices...>, TypeList...> :
            TypeIndex<Indices, TypeList>... {
    using TypeIndex<Indices, TypeList>::value...;
    using TypeIndex<Indices, TypeList>::type...;
};

template<class Variant>
struct TypeIndexer;

template<template<class...> class Variant, class... Ts>
struct TypeIndexer<Variant<Ts...>> :
            TypeIndexer_impl<std::make_index_sequence<sizeof...(Ts)>, Ts...> {
    template<class T>
    static auto constexpr value =
            TypeIndexer_impl<std::make_index_sequence<sizeof...(Ts)>, Ts...>::
                    template value<T>();

    static auto constexpr size = sizeof...(Ts);

    template<size_t I>
    using type = decltype(TypeIndexer_impl<
                          std::make_index_sequence<sizeof...(Ts)>,
                          Ts...>::template type<I>());
};

template<class... OverloadSet>
struct Overload : OverloadSet... {
    using OverloadSet::operator()...;
};

template<class... OverloadSet>
Overload(OverloadSet...) -> Overload<OverloadSet...>;

}    // namespace el

#endif    // ELDERLISP_META_HPP
