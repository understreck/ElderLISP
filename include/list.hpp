#ifndef ELDERLISTP_LIST_HPP
#define ELDERLISTP_LIST_HPP

#include <variant>
#include <tuple>
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
struct List<> : std::tuple<> {};

template<atom_or_list T>
struct List<T> : std::tuple<T> {};

template<atom_or_list T, atom_or_list U>
struct List<T, U> : std::tuple<T, U> {
    constexpr List(T t, U u) : std::tuple<T, U>{t, u}
    {}
};

template<atom_or_list T, atom_or_list U, atom_or_list... Us>
struct List<T, U, Us...> : std::tuple<T, List<U, Us...>> {
    constexpr List(T t, U u, Us... us) :
                std::tuple<T, List<U, Us...>>{t, List<U, Us...>{u, us...}}
    {}
};

template<atom_or_list... Ts>
List(Ts...) -> List<Ts...>;

auto constexpr first_impl(atom_or_list auto)
{
    return List{};
}

template<class T, class... Us>
auto constexpr first_impl(List<T, Us...> l)
{
    return std::get<0>(l);
}

auto constexpr first = [](atom_or_list auto l) {
    return first_impl(l);
};

auto constexpr rest_impl(atom_or_list auto)
{
    return List{};
}

template<class T, class... Us>
auto constexpr rest_impl(List<T, Us...> l)
{
    return std::get<1>(l);
}

auto constexpr rest = [](atom_or_list auto l) {
    return rest_impl(l);
};

auto constexpr cons = []<atom_or_list LHS, atom_or_list RHS>(LHS lhs, RHS rhs) {
    if constexpr(std::is_same_v<LHS, List<>>) {
        return rhs;
    }

    if constexpr(std::is_same_v<RHS, List<>>) {
        return lhs;
    }

    return List{lhs, rhs};
};

template<atom_or_list LHS, atom_or_list RHS>
requires(!std::is_same_v<LHS, RHS>) auto constexpr operator==(LHS, RHS)
{
    return false;
}

#endif    // ELDERLISTP_LIST_HPP
