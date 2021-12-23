#ifndef ELDERLISP_INTERPRETER_HPP
#define ELDERLISP_INTERPRETER_HPP

#include "meta.hpp"
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

auto
interpret_impl(Environment const& global, Scalar const& scalar) -> ScalarOrTuple
{
    return std::visit(
            Overload{
                    [&](Name const& name) -> ScalarOrTuple {
                        if(global.contains(name)) {
                            return global.at(name);
                        }

                        std::cerr << name << " is an unbound name";
                        return NIL{};
                    },
                    [](auto const& scalar) -> ScalarOrTuple {
                        return scalar;
                    }},
            scalar);
}

auto constexpr define = [](Environment, ScalarOrTuple const&) -> ScalarOrTuple {
    return NIL{};
};

auto
interpret_impl(Environment& global, Tuple const& call) -> ScalarOrTuple
{
    if(call.size() < 2) {
        std::cerr << "Not a valid function call";

        return NIL{};
    }

    if(std::holds_alternative<Scalar>(call[0])) {
        std::cerr << "First ";
    }

    return NIL{};
    // auto constexpr interpretArg = [&]() mutable -> ScalarOrTuple {
    // };
}

auto constexpr interpret = [](Environment& global,
                              ScalarOrTuple const& expression) {
    return std::visit(
            [&](auto const& expression) {
                return interpret_impl(global, expression);
            },
            expression);
};

auto
lambda_impl(Environment const&, Scalar const&) -> ScalarOrTuple
{
    std::cerr << "lambda requires a tuple, not a scalar as arguments";

    return NIL{};
}

auto
lambda_impl(Environment const&, Tuple const& args) -> ScalarOrTuple
{
    if(args.size() < 2) {
        std::cerr << "lambda requires a list of unbound names and"
                     " a list containing the function body";
        return NIL{};
    }

    if(std::holds_alternative<Scalar>(args[0])) {
        std::cerr << "lambda requires the first argument to be"
                     " a list of unbound names";

        return NIL{};
    }

    auto const& unboundNames = std::get<Tuple>(args[0]);
    for(auto&& nameSOT : unboundNames) {
        if(auto const& name = nameSOT;
           std::holds_alternative<Scalar>(nameSOT)) {
            if(std::holds_alternative<Name>(std::get<Scalar>(name))) {
                std::cerr << "Unbound arguments list where not all names";

                return NIL{};
            }
        }
    }

    return el::Function{
            [unboundNames = std::get<Tuple>(args[0]), function = args[1]](
                    Environment const& global,
                    ScalarOrTuple const& args) -> ScalarOrTuple {
                if(std::holds_alternative<Scalar>(args)) {
                    std::cerr
                            << "All functions take a list of arguments, not a scalar";

                    return NIL{};
                }

                Tuple const& argList = std::get<Tuple>(args);

                if(unboundNames.size() != argList.size()) {
                    std::cerr << "Number of arguments do not match function";

                    return NIL{};
                }

                auto local = global;
                for(auto i = 0ul; i < unboundNames.size(); i++) {
                    local[std::get<Name>(std::get<Scalar>(unboundNames[i]))] =
                            argList[i];
                }

                return interpret(local, function);
            }};
}

auto constexpr lambda = [](Environment const& global,
                           ScalarOrTuple const& args) {
    return std::visit(
            [&](auto const& args) { return lambda_impl(global, args); },
            args);
};

}    // namespace el

#endif    // ELDERLISP_INTERPRETER_HPP
