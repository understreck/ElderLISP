#ifndef ELDERLISTP_INTERPRETER_HPP
#define ELDERLISTP_INTERPRETER_HPP

#include "list.hpp"
#include "environment.hpp"

auto consteval evaluate(environment auto, atom_or_list auto...);

auto consteval first_impl(atom_or_list auto)
{
    return NIL;
}

template<class T, class... Us>
auto consteval first_impl(List<T, Us...> l)
{
    return std::get<0>(l);
}

auto consteval first(atom_or_list auto l)
{
    return first_impl(l);
};

auto consteval rest_impl(atom_or_list auto)
{
    return NIL;
}

template<class T, class... Us>
auto consteval rest_impl(List<T, Us...> l)
{
    return std::get<1>(l);
}

auto consteval rest(atom_or_list auto l)
{
    return rest_impl(l);
}

template<atom_or_list LHS, atom_or_list RHS>
auto consteval combine(LHS lhs, RHS rhs)
{
    if constexpr(std::is_same_v<LHS, List<>>) {
        return rhs;
    }

    if constexpr(std::is_same_v<List<>, RHS>) {
        return lhs;
    }

    return List{lhs, rhs};
}

template<atom_or_list LHS, atom_or_list RHS>
requires std::is_same_v<LHS, RHS>
auto consteval equal(LHS, RHS)
{
    return True;
}

auto consteval equal(atom_or_list auto, atom_or_list auto)
{
    return False;
}

auto consteval quote(atom_or_list auto aol)
{
    return aol;
}

auto consteval define(
        environment auto env,
        label auto lbl,
        atom_or_list auto aol)
{
    auto [newEnv, result] = evaluate(env, aol);

    return std::pair{push_kvps(newEnv, KeyValuePair{lbl, result}), result};
}

template<label... Labels>
auto consteval lambda_impl(
        environment auto,
        List<Labels...> args,
        list auto funcBody)
{
    return List{CI<LAMBDA>, args, funcBody};
}

// template<environment Env, list L>
// auto consteval lambda_impl(Env outer, List<>, L funcBody, List<>)
//{
// return evaluate(outer, funcBody);
//}

// template<environment Env, list L, atom_or_list Arg>
// auto consteval lambda_impl(
// Env outer,
// List<Label> argName,
// L funcBody,
// List<Arg> arg)
//{
// return evaluate(outer, replace(first(argName), first(arg), funcBody));
//}

// auto consteval lambda =
//[]<environment ReturnEnv>(environment auto outer, list auto... ls) {
// return lambda_impl(outer, ls...);
//};

template<class L>
concept conditional = Length<L>::value == 2;

auto consteval condition_impl(environment auto outer, conditional auto cond)
{
    auto predicateResult = evaluate(outer, first(cond));
    if(equal(predicateResult, True)) {
        return evaluate(outer, rest(cond));
    }

    return NIL;
}

template<conditional Cond, conditional... Cs>
auto consteval condition_impl(environment auto outer, List<Cond, Cs...> conds)
{
    auto cond      = first(conds);
    auto predicate = first(cond);
    auto body      = rest(cond);

    auto predicateResult = evaluate(outer, predicate);
    if(equal(predicateResult, True)) {
        return evaluate(outer, body);
    }

    return condition_impl(outer, rest(conds));
}

template<list... Lists>
auto consteval condition(environment auto outer, List<Lists...> conditionals)
{
    return condition_impl(outer, conditionals);
};

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

auto consteval evaluate(environment auto env, list auto line)
{
    auto [newEnv, function] = evaluate(env, first(line));

    if constexpr(std::is_same_v<decltype(function), CoreInstruction<FIRST>>) {
        return std::pair{newEnv, first(rest(line))};
    }
    if constexpr(std::is_same_v<decltype(function), CoreInstruction<REST>>) {
        return std::pair{newEnv, rest(rest(line))};
    }
    if constexpr(std::is_same_v<decltype(function), CoreInstruction<COMBINE>>) {
        return std::pair{newEnv, combine(first(rest(line)), rest(rest(line)))};
    }
}

auto constexpr result =
        evaluate(Environment{}, List{CI<COMBINE>, Int<5>, Int<4>});
static_assert(result.second == List{Int<5>, Int<4>});

#endif    // ELDERLISTP_INTERPRETER_HPP
