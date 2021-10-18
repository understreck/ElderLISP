#ifndef ELDERLISTP_LIST_HPP
#define ELDERLISTP_LIST_HPP

#include <cstddef>
#include <string_view>
#include <tuple>
#include <variant>

// CoreInstruction
enum CoreInstruction_enum {
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

template<CoreInstruction_enum ci>
struct CoreInstruction : std::integral_constant<CoreInstruction_enum, ci> {};

template<class T>
concept core_instruction = std::is_same_v<T, CoreInstruction<T::value>>;

template<CoreInstruction_enum ci>
auto constexpr CI = CoreInstruction<ci>{};

// Char
template<char c>
struct Character : std::integral_constant<char, c> {};

template<class T>
concept character = std::is_same_v<T, Character<T::value>>;

template<char c>
auto constexpr Char = Character<c>{};

// Integer
template<int i>
struct Integer : std::integral_constant<int, i> {};

template<class T>
concept integer = std::is_same_v<T, Integer<T::value>>;

template<int i>
auto constexpr Int = Integer<i>{};

// Boolean
template<bool b>
struct Boolean : std::integral_constant<bool, b> {};

template<class T>
concept boolean = std::is_same_v<T, Boolean<T::value>>;

template<bool b>
auto constexpr Bool = Boolean<b>{};

auto constexpr True  = Bool<true>;
auto constexpr False = Bool<false>;

// IsSpecalisationOf
template<class T, template<class...> class U>
struct IsSpecalisationOf : std::false_type {};

template<template<class...> class T, class... Args>
struct IsSpecalisationOf<T<Args...>, T> : std::true_type {};

template<class T, template<class...> class U>
concept is_specialisation_of =
        IsSpecalisationOf<std::remove_cvref_t<T>, U>::value;

// Label
template<character... Cs>
struct Label : std::tuple<Cs...> {};

template<char... cs>
auto constexpr Lbl = Label<Character<cs>...>{};

template<class T>
concept label = is_specialisation_of<T, Label>;

// template<class T, class... Us>
// auto constexpr isSameAsOneOf = (std::is_same_v<T, Us> || ...);

template<class T>
concept data_type = core_instruction<T> || character<T> || label<T> || integer<
        T> || boolean<T>;

template<class...>
struct List;

template<>
struct List<> : std::tuple<> {};

auto constexpr NIL = List<>{};

template<class T>
concept atom = std::is_same_v<T, List<>> || data_type<T>;

template<class T>
concept list = is_specialisation_of<T, List>;

template<class... Ts>
concept atom_or_list = ((atom<Ts> || list<Ts>)&&...);

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

template<class... Ts>
List(Ts...) -> List<Ts...>;

template<class T>
struct Length {};

template<class... Ts>
struct Length<List<Ts...>> : std::integral_constant<size_t, sizeof...(Ts)> {};

#endif    // ELDERLISTP_LIST_HPP
