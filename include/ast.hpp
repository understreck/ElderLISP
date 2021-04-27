#ifndef ELDERLISP_AST_HPP
#define ELDERLISP_AST_HPP

#include <deque>
#include <string>
#include <variant>

namespace elderLISP {

namespace ast {
    struct StringLiteral {
        std::string data;
    };
    struct IntegerLiteral {
        long long data;
    };
    struct Equal {};
    struct First {};
    struct Rest {};
    struct Combine {};
    struct Condition {};
    struct Let {};
    struct Quote {};
    struct Lambda {};
    struct Name {
        std::string name;
    };

    using Atom = std::variant<
            StringLiteral,
            IntegerLiteral,
            Equal,
            First,
            Rest,
            Combine,
            Condition,
            Let,
            Quote,
            Lambda,
            Name>;

    struct List : public std::deque<std::variant<List, Atom>> {};

    using Node = std::variant<List, Atom>;
}    // namespace ast

}    // namespace elderLISP

#endif    // ELDERLISP_AST_HPP
