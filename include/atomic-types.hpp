#ifndef ELDERLISP_ATOMIC_TYPES_HPP
#define ELDERLISP_ATOMIC_TYPES_HPP

#include "fixed-string.hpp"

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
    IF,
    LIST,
    MUL,
    SUB,
    ADD,
    DIV,
    MOD,
    LESS,
    GREATER
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
template<long i>
struct Integer : std::integral_constant<long, i> {};

template<class T>
concept integer = std::is_same_v<T, Integer<T::value>>;

template<long i>
auto constexpr Int = Integer<i>{};

auto consteval abs(integer auto i)
{
    if constexpr(i < 0) {
        return Int<-i>;
    }
    else {
        return i;
    }
}

// Rational
template<long n, long d>
struct Rational {
    using Numerator   = Integer<n>;
    using Denominator = Integer<d>;

    Numerator numerator;
    Denominator denominator;
};

template<class T>
concept rational =
        std::is_same_v<T, Rational<T::Numerator::value, T::Denominator::value>>;

template<long n, long d>
auto constexpr Rat = Rational<n, d>{};

auto consteval abs(rational auto r)
{
    return Rat<abs(r.numerator).value, abs(r.denominator).value>;
}

// Boolean
template<bool b>
struct Boolean : std::integral_constant<bool, b> {};

template<class T>
concept boolean = std::is_same_v<T, Boolean<T::value>>;

template<bool b>
auto constexpr Bool = Boolean<b>{};

auto constexpr True  = Bool<true>;
auto constexpr False = Bool<false>;

template<class T>
auto consteval is_core_instruction(T)
{
    return Bool<core_instruction<T>>;
}

// IsSpecalisationOf
template<class T, template<class...> class U>
struct IsSpecalisationOf : std::false_type {};

template<template<class...> class T, class... Args>
struct IsSpecalisationOf<T<Args...>, T> : std::true_type {};

template<class T, template<class...> class U>
concept is_specialisation_of =
        IsSpecalisationOf<std::remove_cvref_t<T>, U>::value;

// Label
template<FixedString string>
struct LabelT : std::integral_constant<decltype(string), string> {};

template<FixedString string>
auto constexpr Lbl = LabelT<string>{};

template<class T>
struct IsLabelT : std::false_type {};

template<FixedString s>
struct IsLabelT<LabelT<s>> : std::true_type {};

template<class T>
concept label = IsLabelT<T>::value;

template<class T>
concept data_type = core_instruction<T> || character<T> || label<T> || integer<
        T> || rational<T> || boolean<T>;

#endif    // ELDERLISP_ATOMIC_TYPES_HPP
