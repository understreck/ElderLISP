#ifndef ELDERLISTP_INTERPRETER_HPP
#define ELDERLISTP_INTERPRETER_HPP

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
requires std::equality_comparable_with<LHS, RHS>
auto consteval equal(LHS lhs, RHS rhs)
{
    return Bool<lhs == rhs>;
}

template<atom_or_list LHS, atom_or_list RHS>
requires(
        !std::equality_comparable_with<LHS, RHS>) auto consteval equal(LHS, RHS)
{
    return False;
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
        core_instruction auto function,
        atom_or_list auto line)
{
    if constexpr(equal(function, CI<FIRST>)) {
        return std::pair{env, first(evaluate(env, line).second)};
    }
    else if constexpr(equal(function, CI<REST>)) {
        return std::pair{env, rest(evaluate(env, line).second)};
    }
    else if constexpr(equal(function, CI<COMBINE>)) {
        return std::pair{
                env,
                combine(evaluate(env, first(line)).second,
                        evaluate(env, rest(line)).second)};
    }
    else if constexpr(equal(function, CI<IF>)) {
        auto predicate = evaluate(env, first(line)).second;
        return std::pair{
                env,
                condition(
                        env,
                        predicate,
                        first(rest(line)),
                        first(rest(rest(line))))};
    }
    else if constexpr(equal(function, CI<EQUAL>)) {
        return std::pair{
                env,
                equal(evaluate(env, first(line)).second,
                      evaluate(env, first(rest(line))).second)};
    }
    else if constexpr(equal(function, CI<ATOM>)) {
        return std::pair{env, atom<decltype(line)>};
    }
    else if constexpr(equal(function, CI<QUOTE>)) {
        return std::pair{env, line};
    }
    else if constexpr(equal(function, CI<DEFINE>)) {
        return define(
                env,
                evaluate(env, first(line)).second,
                evaluate(env, first(rest(line))).second);
    }
    else if constexpr(equal(function, CI<LAMBDA>)) {
        return std::pair{env, lambda(env, line)};
    }
    else if constexpr(equal(function, CI<LIST>)) {
        return std::pair{env, List(line)};
    }
    else if constexpr(equal(function, CI<MUL>)) {
        return std::pair{
                env,
                Int<evaluate(env, first(line)).second
                    * evaluate(env, first(rest(line))).second>};
    }
    else if constexpr(equal(function, CI<SUB>)) {
        return std::pair{
                env,
                Int<evaluate(env, first(line)).second
                    - evaluate(env, first(rest(line))).second>};
    }
}

auto consteval evaluate(environment auto env, list_not_nil auto line)
{
    auto result = evaluate(env, first(line)).second;

    if constexpr(is_core_instruction(result)) {
        return evaluate(env, result, rest(line));
    }
    else if constexpr(is_atom(result)) {
        return std::pair{env, combine(result, rest(line))};
    }
    else if constexpr(is_core_instruction(first(result))) {
        auto newLine = append(result, rest(line));
        return evaluate(env, first(newLine), rest(newLine));
    }
    else {
        return std::pair{env, append(result, rest(line))};
    }
}
#endif    // ELDERLISTP_INTERPRETER_HPP
