#ifndef ELDER_LISP_LEXER_HPP
#define ELDER_LISP_LEXER_HPP

#include <tuple>
#include <utility>

#include <ctll/fixed_string.hpp>
#include <ctll.hpp>
#include <ctre.hpp>

#include "list.hpp"

auto constexpr matches = std::tuple{ctre::match<"\\s*\\(">,
ctre::match<"\\s*lambda">,
ctre::match<"\\s*define">,
ctre::match<"\\s*quote">,
ctre::match<"\\s*atom">,
ctre::match<"\\s*=">,
ctre::match<"\\s*car">,
ctre::match<"\\s*cdr">,
ctre::match<"\\s*cons">,
ctre::match<"\\s*if">,
ctre::match<"\\s*list">,
ctre::match<"\\s*\\*">,
ctre::match<"\\s*-">,
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

//template<size_t position, class... Elements>
//auto consteval parse(c_line auto line, std::tuple<Elements...> elements)
//{}

//auto consteval parse(c_line auto line)
//{}
#endif    // ELDER_LISP_LEXER_HPP
