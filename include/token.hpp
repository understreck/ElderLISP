#ifndef ELDERLISP_TOKEN_HPP
#define ELDERLISP_TOKEN_HPP

#include <string>
#include <variant>

namespace lexer {

struct Whitespace{};

struct LParen {};

struct RParen {};

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
        Whitespace,
        LParen,
        RParen,
        Atom,
        Equals,
        First,
        Rest,
        Combine,
        Condition,
        Let,
        Quote>;

}    // namespace token

#endif    // ELDERLISP_TOKEN_HPP
