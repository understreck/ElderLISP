#ifndef ELDERLISP_AST_HPP
#define ELDERLISP_AST_HPP

#include <vector>
#include <string>
#include <variant>

namespace elderLISP {

namespace ast {
    struct String {
        std::string data;
    };
    struct Integer {
        long long data;
    };
    struct True {};
    struct False {};
    struct NIL {};

    struct Equal {};
    struct Atomic {};
    struct First {};
    struct Rest {};
    struct Combine {};
    struct Condition {};
    struct Let {};
    struct Quote {};
    struct Lambda {};

    struct Symbol {
        std::string name;
    };

    using Value    = std::variant<String, Integer, True, False, NIL>;
    using Function = std::variant<
            Equal,
            Atomic,
            First,
            Rest,
            Combine,
            Condition,
            Let,
            Quote,
            Lambda>;

    using BuiltIn = std::variant<Value, Function, Symbol>;

    struct List;
    using Node = std::variant<List, BuiltIn>;

    struct List : public std::vector<Node> {};
}    // namespace ast

}    // namespace elderLISP

#endif    // ELDERLISP_AST_HPP
