#ifndef ELDERLISTP_LIST_HPP
#define ELDERLISTP_LIST_HPP

#include <ctll.hpp>

#include <cstddef>
#include <tuple>
#include <type_traits>

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
    SUB
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

template<ctll::fixed_string string>
struct SLWrapper {
    static auto constexpr value{string};
};

template<ctll::fixed_string string>
auto constexpr Str = SLWrapper<string>{};

template<class T>
struct IsSLWrapper : std::false_type {};

template<ctll::fixed_string s>
struct IsSLWrapper<SLWrapper<s>> : std::true_type {};

template<class T>
concept string = IsSLWrapper<T>::value;

template<ctll::fixed_string string>
struct LblSLWrapper {
    static auto constexpr value{string};
};

template<ctll::fixed_string string>
auto constexpr Lbl = LblSLWrapper<string>{};

template<class T>
struct IsLblSLWrapper : std::false_type {};

template<ctll::fixed_string s>
struct IsLblSLWrapper<LblSLWrapper<s>> : std::true_type {};

template<class T>
concept label = IsLblSLWrapper<T>::value;

template<class T>
concept data_type = core_instruction<T> || character<T> || string<T> || label<
        T> || integer<T> || boolean<T>;

template<class...>
struct ListT;

template<>
struct ListT<> : std::tuple<> {};

auto constexpr NIL = ListT<>{};

template<class T>
concept nil = std::is_same_v<T, ListT<>>;

template<class...>
struct Input;

template<class T>
concept atom = nil<T> || data_type<T>;

template<class T>
auto consteval is_atom(T)
{
    return Bool<atom<T>>;
}

template<class T>
concept atom_not_nil = atom<T> && !nil<T>;

template<class T>
concept list = is_specialisation_of<T, ListT>;

template<class T>
concept list_not_nil = list<T> && !nil<T>;

template<class... Ts>
concept atom_or_list = ((atom<Ts> || list<Ts>)&&...);

template<class... Ts>
concept atom_or_list_not_nil = ((atom_not_nil<Ts> || list_not_nil<Ts>)&&...);

template<atom_or_list T, atom_or_list U>
struct ListT<T, U> : std::tuple<T, U> {
    constexpr ListT(T t, U u) : std::tuple<T, U>{t, u}
    {}

    constexpr ListT() = default;
};

template<atom_or_list T, atom_or_list U, atom_or_list... Us>
struct ListT<T, U, Us...> : std::tuple<T, ListT<U, Us...>> {
    constexpr ListT(T t, U u, Us... us) :
                std::tuple<T, ListT<U, Us...>>{t, ListT<U, Us...>{u, us...}}
    {}

    constexpr ListT() = default;
};

template<class... Ts>
ListT(Ts...) -> ListT<Ts...>;

auto consteval List()
{
    return NIL;
}

template<atom_or_list_not_nil Element>
auto consteval List(Element element)
{
    return ListT{element, NIL};
}

auto consteval List(atom_or_list auto element, atom_or_list auto... elements)
{
    return ListT{element, elements..., NIL};
}

#endif    // ELDERLISTP_LIST_HPP
