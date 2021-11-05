#ifndef ELDERLISTP_LIST_HPP
#define ELDERLISTP_LIST_HPP

#include <cstddef>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <bit>

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
    MOD
};

template<CoreInstruction_enum ci>
struct CoreInstruction : std::integral_constant<CoreInstruction_enum, ci> {};

template<class T>
concept core_instruction = requires
{
    {
        T::value
        } -> std::same_as<CoreInstruction_enum>;
};

template<CoreInstruction_enum ci>
auto constexpr CI = CoreInstruction<ci>{};

// Char
template<char c>
struct Character : std::integral_constant<char, c> {};

template<class T>
concept character = requires
{
    {
        T::value
        } -> std::same_as<char>;
};

template<char c>
auto constexpr C = Character<c>{};

// Integer
template<int i>
struct Integer : std::integral_constant<int, i> {};

template<class T>
concept integer = requires
{
    {
        T::value
        } -> std::same_as<int>;
};

template<long i>
auto constexpr Int = Integer<i>{};

// Boolean
template<bool b>
struct Boolean : std::integral_constant<bool, b> {};

template<class T>
concept boolean = requires
{
    {
        T::value
        } -> std::same_as<bool>;
};

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
template<size_t N>
struct FixedString : std::array<char, N> {
    template<size_t... indices>
    consteval FixedString(
            char const (&str)[N + 1],
            std::index_sequence<indices...>) :
                std::array<char, N>{str[indices]...}
    {}

    consteval FixedString(char const (&str)[N + 1]) :
                FixedString{str, std::make_index_sequence<N>{}}
    {}

    template<class... C>
    constexpr FixedString(C... cs) : std::array<char, N>{cs...}
    {}

    constexpr operator std::string_view() const
    {
        return std::string_view{this->data(), N};
    }
};

template<size_t N>
FixedString(char const (&)[N]) -> FixedString<N - 1>;

template<
        size_t begin,
        size_t end,
        FixedString line,
        template<auto>
        class Wrapper>
auto consteval substring(Wrapper<line>)
{
    return []<size_t... indices>(std::index_sequence<indices...>)
    {
        return FixedString<end - begin>{line[indices + begin]...};
    }
    (std::make_index_sequence<end - begin>{});
}

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
        T> || boolean<T>;

template<class...>
struct ListT;

template<>
struct ListT<> : std::tuple<> {};

auto constexpr NIL = ListT<>{};

template<class T>
concept nil = std::is_same_v<T, ListT<>>;

template<class...>
struct Procedure;

template<class T>
concept procedure = is_specialisation_of<T, Procedure>;

template<class T>
concept atom = nil<T> || data_type<T> || procedure<T>;

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
