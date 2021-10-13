#ifndef ELDERLISTP_TOKEN_HPP
#define ELDERLISTP_TOKEN_HPP

#include <variant>
#include <string_view>

enum class CoreInstruction {
    LAMBDA,
    DEFINE,
    QUOTE,
    ATOM,
    EQUAL,
    FIRST,
    REST,
    COMBINE,
    CONDITION
};

template<class T, template<class...> class U>
struct IsSpecalisationOf : std::false_type {};

template<template<class...> class T, class... Args>
struct IsSpecalisationOf<T<Args...>, T> : std::true_type {};

template<class T, template<class...> class U>
auto constexpr isSpecalisationOf = IsSpecalisationOf<T, U>::value;

struct Atom : std::variant<CoreInstruction, bool, int> {};

template<class...>
struct List;

template<class... Ts>
concept atom_or_list =
        ((std::is_same_v<Ts, Atom> || isSpecalisationOf<Ts, List>)&&...);

template<>
struct List<> {};

template<atom_or_list T>
struct List<T> {
    T first;
};

template<atom_or_list T, atom_or_list... Us>
struct List<T, Us...> {
    constexpr List(T t, Us... us) : first{t}, rest{us...}
    {}

    T first;
    List<Us...> rest;
};

template<atom_or_list T, atom_or_list... Us>
List(T, Us...) -> List<T, Us...>;

auto constexpr first(atom_or_list auto)
{
    return List{};
}

template<class T, class... Us>
auto constexpr first(List<T, Us...> l)
{
    return l.first;
}

auto constexpr rest(atom_or_list auto)
{
    return List{};
}

template<class T, class... Us>
auto constexpr rest(List<T, Us...> l)
{
    return l.rest;
}
#endif    // ELDERLISTP_TOKEN_HPP
