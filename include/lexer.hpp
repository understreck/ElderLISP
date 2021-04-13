#ifndef ELDERLISP_LEXER_HPP
#define ELDERLISP_LEXER_HPP

#include <string>
#include <variant>
#include <deque>

namespace lexer {

struct LPeren {};

struct RPeren {};

struct Atom {
    std::string name;
};

struct Equals {};

struct First {};

struct Rest {};

struct Combine {};

struct Condition {};

struct Let {};

struct Quote {};

using Token = std::variant<
        LPeren,
        RPeren,
        Atom,
        Equals,
        First,
        Rest,
        Combine,
        Condition,
        Let,
        Quote>;
auto
tokenize(
        std::string::const_iterator begin,
        std::string::const_iterator end,
        std::deque<Token> tokens) -> std::deque<Token>;
}    // namespace Lexer

#endif    // ELDERLISP_LEXER_HPP
