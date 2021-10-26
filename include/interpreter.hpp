#ifndef ELDERLISTP_INTERPRETER_HPP
#define ELDERLISTP_INTERPRETER_HPP

#include "lexer.hpp"
#include "list.hpp"
#include "environment.hpp"

#include <type_traits>

auto consteval evaluate(environment auto, atom_or_list auto...);

auto consteval quote(atom_or_list auto aol)
{
    return aol;
}

auto consteval first(list_not_nil auto l)
{
    return std::get<0>(l);
}

auto consteval rest(list auto l)
{
    return std::get<1>(l);
}

template<atom_or_list LHS, atom_or_list RHS>
auto consteval equal(LHS, RHS)
{
    return Bool<std::is_same_v<LHS, RHS>>;
}

auto consteval combine(atom auto lhs, atom_not_nil auto rhs)
{
    return ListT{lhs, rhs};
}

auto consteval combine(atom_or_list auto lhs, nil auto)
{
    return List(lhs);
}

template<size_t ElementsLeft>
requires(ElementsLeft == 1) auto consteval unroll_right(
        atom_or_list auto lhs,
        atom_or_list auto lastElement,
        atom_or_list auto... elements)
{
    return ListT{lhs, elements..., lastElement};
}

template<size_t ElementsLeft>
requires(ElementsLeft >= 2) auto consteval unroll_right(
        atom_or_list auto lhs,
        list_not_nil auto restOfList,
        atom_or_list auto... elements)
{
    return unroll_right<ElementsLeft - 1>(
            lhs,
            rest(restOfList),
            elements...,
            first(restOfList));
}

template<atom_or_list... Elements>
requires(sizeof...(Elements) >= 2) auto consteval combine(
        atom_or_list auto lhs,
        ListT<Elements...> rhs)
{
    return unroll_right<sizeof...(Elements) - 1>(lhs, rest(rhs), first(rhs));
}

auto consteval length(nil auto)
{
    return 0;
}

auto consteval length(atom_not_nil auto)
{
    return 1;
}

auto consteval length(list_not_nil auto l)
{
    return 1 + length(rest(l));
}

auto consteval append(nil auto, list auto b)
{
    return b;
}

auto consteval append(list_not_nil auto a, list auto b)
{
    return combine(first(a), append(rest(a), b));
}

auto consteval condition(
        environment auto outer,
        boolean auto predicate,
        atom_or_list auto ifTrue,
        atom_or_list auto ifFalse)
{
    if constexpr(predicate) {
        return evaluate(outer, ifTrue).second;
    }
    else {
        return evaluate(outer, ifFalse).second;
    }
};

auto consteval define(
        environment auto env,
        label auto lbl,
        atom_or_list auto aol)
{
    auto [newEnv, result] = evaluate(env, aol);

    return std::pair{
            push_kvps(newEnv, std::tuple{KeyValuePair{lbl, result}}),
            result};
}

template<label L>
auto consteval replace(L, atom_or_list auto replacement, L)
{
    return replacement;
}

auto consteval replace(label auto, atom_or_list auto, atom auto body)
{
    return body;
}

auto consteval replace(
        label auto name,
        atom_or_list auto replacement,
        list_not_nil auto body)
{
    return combine(
            replace(name, replacement, first(body)),
            replace(name, replacement, rest(body)));
}

template<label ArgName, atom_or_list FBody>
auto consteval lambda(environment auto, ListT<ArgName, FBody> funcExpr)
{
    return combine(CI<LAMBDA>, funcExpr);
}

auto consteval lambda(environment auto outer, list_not_nil auto funcExpr)
{
    if constexpr(length(funcExpr) == 2) {
        return combine(CI<LAMBDA>, funcExpr);
    }
    else if constexpr(length(funcExpr) == 3) {
        auto argNames = first(funcExpr);
        auto body     = first(rest(funcExpr));
        auto args     = first(rest(rest(funcExpr)));

        auto newBody =
                replace(first(argNames),
                        evaluate(outer, first(args)).second,
                        body);

        if constexpr(equal(rest(argNames), NIL)) {
            return evaluate(outer, newBody).second;
        }
        else if constexpr(equal(rest(args), NIL)) {
            return List(CI<LAMBDA>, rest(argNames), newBody);
        }
        else {
            return lambda(outer, List(rest(argNames), newBody, rest(args)));
        }
    }
}

template<atom Atom>
auto consteval evaluate(environment auto env, Atom a)
{
    if constexpr(label<Atom>) {
        return std::pair{env, find(env, a)};
    }
    else {
        return std::pair{env, a};
    }
}

auto consteval evaluate(
        environment auto env,
        CoreInstruction<FIRST>,
        atom_or_list auto args)
{
    if constexpr(length(args) != 1) {
        static_assert(
                std::is_same_v<decltype(args), void>,
                "Incorrect amount of arguments for First");
    }
    else {
        return std::pair{env, first(evaluate(env, args).second)};
    }
}

auto consteval evaluate(
        environment auto env,
        CoreInstruction<REST>,
        atom_or_list auto args)
{
    if constexpr(length(args) != 1) {
        static_assert(
                std::is_same_v<decltype(args), void>,
                "Incorrect amount of arguments for Rest");
    }
    else {
        return std::pair{env, rest(evaluate(env, args).second)};
    }
}

auto consteval evaluate(
        environment auto env,
        CoreInstruction<COMBINE>,
        atom_or_list auto args)
{
    if constexpr(length(args) != 2) {
        static_assert(
                std::is_same_v<decltype(args), void>,
                "Incorrect amount of arguments for Combine");
    }
    else {
        return std::pair{
                env,
                combine(evaluate(env, first(args)).second,
                        evaluate(env, rest(args)).second)};
    }
}

auto consteval evaluate(
        environment auto env,
        CoreInstruction<IF>,
        atom_or_list auto args)
{
    if constexpr(length(args) != 3) {
        static_assert(
                std::is_same_v<decltype(args), void>,
                "Incorrect amount of arguments for If");
    }
    else {
        auto predicate = first(args);
        auto ifTrue    = first(rest(args));

        if constexpr(atom<decltype(rest(rest(args)))>) {
            static_assert(
                    std::is_same_v<decltype(args), void>,
                    "Ifs arguments need to be NIL terminated");
        }
        else {
            auto ifFalse = first(rest(rest(args)));

            return std::pair{
                    env,
                    condition(
                            evaluate(env, predicate).second,
                            ifTrue,
                            ifFalse)};
        }
    }
}

template<list List>
auto consteval evaluate(
        environment auto env,
        core_instruction auto function,
        List args)
{
    else if constexpr(equal(function, CI<EQUAL>))
    {
        return std::pair{
                env,
                equal(evaluate(env, first(args)).second,
                      evaluate(env, first(rest(args))).second)};
    }
    else if constexpr(equal(function, CI<ATOM>))
    {
        return std::pair{env, atom<decltype(args)>};
    }
    else if constexpr(equal(function, CI<QUOTE>))
    {
        return std::pair{env, args};
    }
    else if constexpr(equal(function, CI<DEFINE>))
    {
        return define(
                env,
                evaluate(env, first(args)).second,
                evaluate(env, first(rest(args))).second);
    }
    else if constexpr(equal(function, CI<LAMBDA>))
    {
        return std::pair{env, lambda(env, args)};
    }
    else if constexpr(equal(function, CI<LIST>))
    {
        return std::pair{env, List(args)};
    }
    else if constexpr(equal(function, CI<MUL>))
    {
        return std::pair{
                env,
                Int<evaluate(env, first(args)).second
                    * evaluate(env, first(rest(args))).second>};
    }
    else if constexpr(equal(function, CI<SUB>))
    {
        return std::pair{
                env,
                Int<evaluate(env, first(args)).second
                    - evaluate(env, first(rest(args))).second>};
    }
    else if constexpr(equal(function, CI<ADD>))
    {
        return std::pair{
                env,
                Int<evaluate(env, first(args)).second
                    + evaluate(env, first(rest(args))).second>};
    }
    else if constexpr(equal(function, CI<DIV>))
    {
        return std::pair{
                env,
                Int<evaluate(env, first(args)).second
                    / evaluate(env, first(rest(args))).second>};
    }
    else if constexpr(equal(function, CI<MOD>))
    {
        return std::pair{
                env,
                Int<evaluate(env, first(args)).second
                    % evaluate(env, first(rest(args))).second>};
    }
}

auto consteval evaluate(environment auto env, list_not_nil auto line)
{
    auto f = first(line);

    if constexpr(is_core_instruction(f)) {
        return evaluate(env, f, rest(line));
    }
    else if(label<decltype(f)>) {
        auto newLine = append(find(env, f), rest(line));
        return evaluate(env, newLine);
    }
    else {
        static_assert(
                std::is_same_v<decltype(f), void>,
                "First element of list is not a core instruction");
        return;
    }
}
#endif    // ELDERLISTP_INTERPRETER_HPP
