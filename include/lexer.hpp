#ifndef ELDERLISP_LEXER_HPP
#define ELDERLISP_LEXER_HPP

#include <token.hpp>

#include <string>
#include <variant>
#include <deque>

namespace lexer {
auto
tokenize(
        std::string::const_iterator begin,
        std::string::const_iterator end,
        std::deque<Token> tokens) -> std::deque<Token>;
}    // namespace Lexer

#endif    // ELDERLISP_LEXER_HPP
