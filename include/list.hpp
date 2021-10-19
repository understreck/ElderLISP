#ifndef ELDERLISTP_LIST_HPP
#define ELDERLISTP_LIST_HPP

#include <cstddef>
#include <iostream>
#include <string_view>
#include <string>
#include <tuple>
#include <type_traits>
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
    CONDITION,
    OUT,
    IN
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
auto constexpr C = Character<c>{};

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
template<size_t N>
struct StringLiteral : std::array<char, N> {
    consteval StringLiteral(char const (&str)[N]) :
                std::array<char, N>{std::to_array(str)}
    {}
};

template<size_t N>
StringLiteral(char const (&)[N]) -> StringLiteral<N>;

template<StringLiteral string>
struct SLWrapper {
    static auto constexpr value{string};
};

template<StringLiteral string>
auto constexpr Str = SLWrapper<string>{};

template<class T>
struct IsSLWrapper : std::false_type {};

template<StringLiteral s>
struct IsSLWrapper<SLWrapper<s>> : std::true_type {};

template<class T>
concept string = IsSLWrapper<T>::value;

template<StringLiteral string>
struct LblSLWrapper {
    static auto constexpr value{string};
};

template<StringLiteral string>
auto constexpr Lbl = LblSLWrapper<string>{};

template<class T>
struct IsLblSLWrapper : std::false_type {};

template<StringLiteral s>
struct IsLblSLWrapper<LblSLWrapper<s>> : std::true_type {};

template<class T>
concept label = IsLblSLWrapper<T>::value;

template<class T>
concept data_type = core_instruction<T> || character<T> || string<T> || label<
        T> || integer<T> || boolean<T>;

template<class...>
struct List;

template<class>
struct Output;

template<character... Cs>
struct Output<List<Cs...>> : List<Cs...> {
    auto
    operator()() const
    {
        ((std::cout << Cs::value), ...);
    }
};

template<StringLiteral s>
struct Output<SLWrapper<s>> : SLWrapper<s> {
    using SLWrapper<s>::value;
    auto
    operator()() const
    {
        for(auto const c : value) {
            std::cout << c;
        }
    }
};

template<character... Cs>
Output(List<Cs...>) -> Output<List<Cs...>>;

template<StringLiteral s>
Output(SLWrapper<s>) -> Output<SLWrapper<s>>;

template<>
struct List<> : std::tuple<> {};

auto constexpr NIL = List<>{};

template<class...>
struct Input;

template<class T>
concept atom = std::is_same_v<T, List<>> || data_type<
        T> || is_specialisation_of<T, Output> || is_specialisation_of<T, Input>;

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
