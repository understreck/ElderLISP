#ifndef ELDERLISP_AST_HPP
#define ELDERLISP_AST_HPP

#include <vector>
#include <string>
#include <variant>

namespace elderLISP {

namespace ast {
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
            Equal,
            First,
            Rest,
            Combine,
            Condition,
            Let,
            Quote,
            Lambda,
            Name>;

    struct List : public std::vector<std::variant<List, Atom>> {};
}    // namespace ast

}    // namespace elderLISP

#endif    // ELDERLISP_AST_HPP
