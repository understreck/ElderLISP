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

auto
equal_impl(Scalar const&) -> ScalarOrTuple
{
    std::cerr << "eq? requires a list or a pair of arguments,\nnot an atom";
    return NIL{};
};

auto
equal_impl(Tuple const& args) -> ScalarOrTuple
{
    if(args.size() < 2) {
        std::cerr << "eq? requires a list or a pair of arguments,\nnot an atom";

        return NIL{};
    }

    auto it = std::ranges::adjacent_find(
            args,
            [](auto const& lhs, auto const& rhs) { return lhs != rhs; });

    return it == args.end();
}

auto constexpr equal = [](ScalarOrTuple const& args) -> ScalarOrTuple {
    return std::visit([](auto const& args) { return equal_impl(args); }, args);
};

auto
car_impl(Scalar const&) -> ScalarOrTuple
{
    std::cerr << "car requires a list or pair as argument,\nnot an atom";

    return NIL{};
}

auto
car_impl(Tuple const& args) -> ScalarOrTuple
{
    if(args.size() < 2) {
        std::cerr << "car requires a list or pair as argument,\nnot an atom";

        return NIL{};
    }

    return args[0];
}

auto constexpr car = [](ScalarOrTuple const& args) -> ScalarOrTuple {
    return std::visit([](auto const& args) { return car_impl(args); }, args);
};

auto
cdr_impl(Scalar const&) -> ScalarOrTuple
{
    std::cerr << "cdr requires a list or pair as argument,\nnot an atom";

    return NIL{};
}

auto
cdr_impl(Tuple const& args) -> ScalarOrTuple
{
    if(args.size() < 2) {
        std::cerr << "cdr requires a list or pair as argument,\nnot an atom";

        return NIL{};
    }

    if(args.size() == 2) {
        return args[1];
    }

    return [&] {
        auto rest = Tuple();
        rest.reserve(args.size() - 1);

        std::for_each(args.begin() + 1, args.end(), [&](auto const& element) {
            rest.push_back(element);
        });

        return rest;
    }();
}

auto constexpr cdr = [](ScalarOrTuple const& args) -> ScalarOrTuple {
    return std::visit([](auto const& args) { return cdr_impl(args); }, args);
};

auto constexpr quote = [](ScalarOrTuple const& args) -> ScalarOrTuple {
    return args;
};

}    // namespace el

#endif    // ELDERLISP_AST_HPP
