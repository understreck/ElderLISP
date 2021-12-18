#ifndef ELDERLISP_AST_HPP
#define ELDERLISP_AST_HPP

#include <iostream>
#include <variant>
#include <functional>
#include <vector>
#include <algorithm>

namespace el {

// forward declarations
struct Function;

// definitions
struct NIL {
    friend bool constexpr
    operator==(NIL const&, NIL const&) = default;
};

struct Rational {
    long numer;    // numerator
    long denom;    // denominator

    friend auto constexpr
    operator==(Rational const&, Rational const&) -> bool = default;

    friend auto constexpr
    operator<(Rational const& lhs, Rational const& rhs) -> bool
    {
        return lhs.numer * rhs.denom < rhs.numer * lhs.denom;
    }

    friend auto constexpr
    operator>(Rational const& lhs, Rational const& rhs) -> bool
    {
        return lhs.numer * rhs.denom > rhs.numer * lhs.denom;
    }

    friend auto constexpr
    operator>=(Rational const& lhs, Rational const& rhs) -> bool
    {
        return lhs > rhs || lhs == rhs;
    }

    friend auto constexpr
    operator<=(Rational const& lhs, Rational const& rhs) -> bool
    {
        return lhs < rhs || lhs == rhs;
    }
};

using Scalar = std::variant<Rational, long, Function, bool, char, NIL>;

struct Tuple : std::vector<std::variant<Scalar, Tuple>> {
    using Base = std::vector<std::variant<Scalar, Tuple>>;
    using Base::Base;

    friend auto
    operator==(NIL const, Tuple const& rhs) -> bool
    {
        return rhs.empty();
    }

    friend auto
    operator==(Tuple const& rhs, NIL const) -> bool
    {
        return rhs.empty();
    }
};

using ScalarOrTuple = std::variant<Scalar, Tuple>;

using FunctionSig = ScalarOrTuple(ScalarOrTuple);

struct Function : std::function<FunctionSig> {
    friend auto
    operator==(Function const& lhs, Function const& rhs)
    {
        return lhs.target_type() == rhs.target_type();
    }

    friend auto
    operator!=(Function const& lhs, Function const& rhs)
    {
        return !(lhs == rhs);
    }
};

}    // namespace el

#endif    // ELDERLISP_AST_HPP
