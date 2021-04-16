#ifndef ELDERLISP_TOKEN_HPP
#define ELDERLISP_TOKEN_HPP

#include <string>
#include <variant>

namespace elderLISP {

namespace token {
    struct Whitespace {};

    struct StringLiteral {
        std::string data;
    };

    struct IntegerLiteral {
        long long data;
    };

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
            StringLiteral,
            IntegerLiteral,
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

}    // namespace elderLISP

#endif    // ELDERLISP_TOKEN_HPP
