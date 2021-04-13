#ifndef ELDERLISP_AST_HPP
#define ELDERLISP_AST_HPP

#include <string>
#include <variant>

namespace token {

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

#endif    // ELDERLISP_AST_HPP
