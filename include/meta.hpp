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
};

template<class IndexSequence, class... TypeList>
struct TypeIndexer_impl;

template<std::size_t... Indices, class... TypeList>
struct TypeIndexer_impl<std::index_sequence<Indices...>, TypeList...> :
            TypeIndex<Indices, TypeList>... {
    using TypeIndex<Indices, TypeList>::value...;
};

template<template<class...> class Variant, class U = void>
struct TypeIndexer;

template<template<class...> class Variant, class... Ts>
struct TypeIndexer<Variant, Variant<Ts...>> :
            TypeIndexer_impl<std::make_index_sequence<sizeof...(Ts)>, Ts...> {
    using TypeIndexer_impl<std::make_index_sequence<sizeof...(Ts)>, Ts...>::
            value;
};

}    // namespace el

#endif    // ELDERLISP_META_HPP
