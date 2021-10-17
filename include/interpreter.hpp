#ifndef ELDERLISTP_INTERPRETER_HPP
#define ELDERLISTP_INTERPRETER_HPP

#include "list.hpp"
//#include "environment.hpp"

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

auto consteval define(environment auto env, Label lbl, atom_or_list auto aol)
{
    return std::pair{push_kvps(env, KeyValuePair{lbl, aol}), lbl};
}

template<environment Env, label... Labels, list L>
auto consteval lambda_impl(Env outer, List<Labels...> args, L funcBody)
{
    return std::pair{outer, List{CoreInstruction::LAMBDA, args, funcBody}};
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

template<atom_or_list Predicate, atom_or_list Body>
auto consteval condition_impl(
        environment auto outer,
        List<Predicate, Body> conditional)
{
    auto predicateResult = evaluate(outer, first(conditional));
 if(
}

template<list... Lists>
auto consteval condition(environment auto outer, List<Lists...> conditions)
{
    return condition_impl(outer, conditions);
};

#endif    // ELDERLISTP_INTERPRETER_HPP
