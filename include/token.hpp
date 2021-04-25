#ifndef ELDERLISP_TOKEN_HPP
#define ELDERLISP_TOKEN_HPP

#include <string>
#include <variant>

namespace elderLISP {

namespace token {
    struct StringLiteral {
        std::string data;
    };
    struct IntegerLiteral {
        long long data;
    };
    struct Name {
        std::string name;
    };
    struct LParen {};
    struct RParen {};
    struct Equal {};
    struct First {};
    struct Rest {};
    struct Combine {};
    struct Condition {};
    struct Let {};
    struct Lambda {};
    struct Quote {};

    using Token = std::variant<
            StringLiteral,
            IntegerLiteral,
            LParen,
            RParen,
            Name,
            Equal,
            First,
            Rest,
            Combine,
            Condition,
            Lambda,
            Let,
            Quote>;
}    // namespace token

}    // namespace elderLISP

#endif    // ELDERLISP_TOKEN_HPP
