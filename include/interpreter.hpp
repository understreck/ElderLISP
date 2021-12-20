#ifndef ELDERLISP_INTERPRETER_HPP
#define ELDERLISP_INTERPRETER_HPP

#include "ast.hpp"

namespace el {

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

auto constexpr equal = [](Environment const&,
                          ScalarOrTuple const& args) -> ScalarOrTuple {
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

auto constexpr car = [](Environment const&,
                        ScalarOrTuple const& args) -> ScalarOrTuple {
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

auto constexpr cdr = [](Environment const&,
                        ScalarOrTuple const& args) -> ScalarOrTuple {
    return std::visit([](auto const& args) { return cdr_impl(args); }, args);
};

auto constexpr quote = [](Environment const&,
                          ScalarOrTuple const& args) -> ScalarOrTuple {
    return args;
};

auto
atom_impl(Scalar const&) -> ScalarOrTuple
{
    return true;
}

auto
atom_impl(Tuple const& args) -> ScalarOrTuple
{
    return args.empty();
}

auto constexpr atom = [](Environment const&,
                         ScalarOrTuple const& args) -> ScalarOrTuple {
    return std::visit([](auto const& args) { return atom_impl(args); }, args);
};



auto constexpr lambda = [](Environment const& global,
                           ScalarOrTuple const& args) {
    return std::visit(
            [&global](ScalarOrTuple const& args) {
                return lambda_impl(global, args);
            },
            args);
};

}    // namespace el

#endif    // ELDERLISP_INTERPRETER_HPP
