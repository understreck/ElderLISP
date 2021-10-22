#ifndef ELDER_LISP_LEXER_HPP
#define ELDER_LISP_LEXER_HPP

#include <ctll.hpp>
#include <ctre.hpp>

template<ctll::fixed_string FS>
struct str {};

auto constexpr a = str<"hi">{};

#endif    // ELDER_LISP_LEXER_HPP
