#ifndef ELDER_LISP_LEXER_HPP
#define ELDER_LISP_LEXER_HPP

#include <tuple>
#include <utility>

#include <ctll/fixed_string.hpp>
#include <ctll.hpp>
#include <ctre.hpp>

#include "list.hpp"

auto constexpr matches = std::tuple{
        ctre::match<"\\s*lambda(\\s|\\(|\\))">,    // CI<LAMBDA>
        ctre::match<"\\s*define(\\s|\\(|\\))">,    // CI<DEFINE>
        ctre::match<"\\s*quote(\\s|\\(|\\))">,     // CI<QUOTE>
        ctre::match<"\\s*atom(\\s|\\(|\\))">,      // CI<ATOM>
        ctre::match<"\\s*=(\\s|\\(|\\))">,         // CI<EQUAL
        ctre::match<"\\s*car(\\s|\\(|\\))">,       // CI<FIRST>
        ctre::match<"\\s*cdr(\\s|\\(|\\))">,       // CI<REST>
        ctre::match<"\\s*cons(\\s|\\(|\\))">,      // CI<COMBINE>
        ctre::match<"\\s*if(\\s|\\(|\\))">,        // CI<IF>
        ctre::match<"\\s*list(\\s|\\(|\\))">,      // CI<LIST>
        ctre::match<"\\s*\\*(\\s|\\(|\\))">,       // CI<MUL>
        ctre::match<"\\s*-(\\s|\\(|\\))">,         // CI<SUB>
        ctre::match<"\\s*\\+(\\s|\\(|\\))">,       // CI<ADD>
        ctre::match<"\\s*\\/(\\s|\\(|\\))">,       // CI<DIV>
        ctre::match<"\\s*%(\\s|\\(|\\))">,         // CI<MOD>
        ctre::match<"\\s*(T|F)(\\s|\\(|\\))">,     // Bool
        ctre::match<"\\s*'(\\S)'">,                // Char
        ctre::match<"\\s*(\\d+)">,                 // Digit
        ctre::match<"\\s*\\(">,                    // Open List
        ctre::match<"\\s*\\)">,                    // Close list
        ctre::match<"\\s*(\\S+)">,                 // Label
        ctre::match<"\\s*$">,                      // Trailing whitespace
};

template<ctll::fixed_string m_string>
struct LineT : std::integral_constant<decltype(m_string), m_string> {};

template<ctll::fixed_string string>
auto constexpr Line = LineT<string>{};

template<class T>
struct IsLineT : std::false_type {};

template<ctll::fixed_string s>
struct IsLineT<LineT<s>> : std::true_type {};

template<class T>
concept c_line = IsLineT<T>::value;

// template<size_t position, class... Elements>
// auto consteval parse(c_line auto line, std::tuple<Elements...> elements)
//{}

template<size_t position, size_t tupleIndex = 0>
auto consteval matchNext(c_line auto line)
{
    auto constexpr s = std::basic_string_view{
            line.value.begin() + position,
            line.value.end()};
    if constexpr(auto constexpr match = std::get<tupleIndex>(matches)(s)) {
        struct ret {
            size_t index;
            decltype(match) match;
        };
        return ret{tupleIndex, match};
    }
    else {
        return matchNext<position, tupleIndex + 1>(line);
    }
}

template<size_t position = 0>
auto consteval parse(c_line auto line)
{
    auto constexpr m = matchNext<position>(line);

    if constexpr(m.index == LAMBDA) {
        return CI<LAMBDA>;
    }
    else if constexpr(m.index == DEFINE) {
        return CI<DEFINE>;
    }
    else if constexpr(m.index == QUOTE) {
        return CI<QUOTE>;
    }
    else if constexpr(m.index == ATOM) {
        return CI<ATOM>;
    }
    else if constexpr(m.index == EQUAL) {
        return CI<EQUAL>;
    }
    else if constexpr(m.index == FIRST) {
        return CI<FIRST>;
    }
    else if constexpr(m.index == REST) {
        return CI<REST>;
    }
    else if constexpr(m.index == COMBINE) {
        return CI<COMBINE>;
    }
    else if constexpr(m.index == IF) {
        return CI<IF>;
    }
    else if constexpr(m.index == LIST) {
        return CI<LIST>;
    }
    else if constexpr(m.index == MUL) {
        return CI<MUL>;
    }
    else if constexpr(m.index == SUB) {
        return CI<SUB>;
    }
    else if constexpr(m.index == ADD) {
        return CI<ADD>;
    }
    else if constexpr(m.index == DIV) {
        return CI<DIV>;
    }
    else if constexpr(m.index == MOD) {
        return CI<MOD>;
    }
}

auto constexpr p = parse(Line<"lambda)">);
#endif    // ELDER_LISP_LEXER_HPP
