#ifndef ELDERLISP_ACTION_HPP
#define ELDERLISP_ACTION_HPP

#include "token.hpp"
#include "value.hpp"

#include <variant>

namespace elderLISP {

namespace action {
    using token::StringLiteral;
    using token::IntegerLiteral;
    using token::Equals;
    using token::First;
    using token::Rest;
    using token::Combine;
    using token::Condition;
    using token::Let;
    using token::Quote;

    using value::Value;
    struct OpenList {};
    struct CloseList {};

    using Action = std::variant<
            OpenList,
            CloseList,
            Equals,
            First,
            Rest,
            Combine,
            Condition,
            Let,
            Value,
            Quote>;

}    // namespace action

}    // namespace elderLISP

#endif    // ELDERLISP_ACTION_HPP
