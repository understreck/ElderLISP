#ifndef ELDERLISP_LEXER_HPP
#define ELDERLISP_LEXER_HPP

#include <iterator>
#include <sstream>
#include <variant>
#include <deque>

struct Atom {
    std::string name;
};

struct LPeren {};

struct RPeren {};

struct NIL {};

using Token = std::variant<NIL, LPeren, RPeren, Atom>;

auto
nextToken(std::string::const_iterator input, std::string::const_iterator end)
        -> std::tuple<Token, std::string::const_iterator>;

auto
tokenize(
        std::string::const_iterator begin,
        std::string::const_iterator end,
        std::deque<Token> tokens) -> std::deque<Token>;

#endif    // ELDERLISP_LEXER_HPP
