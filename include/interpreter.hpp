#ifndef ELDERLISTP_INTERPRETER_HPP
#define ELDERLISTP_INTERPRETER_HPP

#include "list.hpp"
#include "environment.hpp"

auto consteval evaluate(environment auto, atom_or_list auto...);

auto consteval first(atom_or_list auto)
{
    return NIL;
}

template<class T, class... Us>
auto consteval first(List<T, Us...> l)
{
    return std::get<0>(l);
}

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
    auto predicate = first(cond);
    auto body      = rest(cond);

    auto [newEnv, predicateResult] = evaluate(outer, predicate);
    if constexpr(std::is_same_v<decltype(predicateResult), Boolean<true>>) {
        return evaluate(newEnv, body).second;
    }
    else {
        return NIL;
    }
}

template<conditional Cond, conditional... Cs>
auto consteval condition_impl(environment auto outer, List<Cond, Cs...> conds)
{
    auto cond      = first(conds);
    auto predicate = first(cond);
    auto body      = rest(cond);

    auto [newEnv, predicateResult] = evaluate(outer, predicate)();
    if constexpr(std::is_same_v<decltype(predicateResult), Boolean<true>>) {
        return evaluate(newEnv, body)().second;
    }
    else {
        return condition_impl(outer, rest(conds));
    }
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
        return [=]() {
            return std::pair{env, find(env, a)};
        };
    }
    else {
        return [=]() {
            return std::pair{env, a};
        };
    }
}

template<list Line>
requires(!std::is_same_v<Line, List<>>) auto consteval evaluate(
        environment auto env,
        Line line)
{
    auto [nE, f]  = evaluate(env, first(line))();
    auto newEnv   = nE;
    auto function = f;

    if constexpr(std::is_same_v<decltype(function), CoreInstruction<FIRST>>) {
        return [=]() {
            return std::pair{
                    newEnv,
                    first(evaluate(newEnv, rest(line))().second)};
        };
    }
    else if constexpr(std::is_same_v<
                              decltype(function),
                              CoreInstruction<REST>>) {
        return [=]() {
            return std::pair{
                    newEnv,
                    rest(evaluate(newEnv, rest(line))().second)};
        };
    }
    else if constexpr(std::is_same_v<
                              decltype(function),
                              CoreInstruction<COMBINE>>) {
        return [=]() {
            return std::pair{
                    newEnv,
                    combine(evaluate(newEnv, first(rest(line)))().second,
                            evaluate(newEnv, rest(rest(line)))().second)};
        };
    }
    else if constexpr(std::is_same_v<
                              decltype(function),
                              CoreInstruction<CONDITION>>) {
        return [=]() {
            return std::pair{newEnv, condition(newEnv, rest(line))};
        };
    }
    else if constexpr(std::is_same_v<
                              decltype(function),
                              CoreInstruction<EQUAL>>) {
        return [=]() {
            return std::pair{
                    newEnv,
                    equal(evaluate(newEnv, first(rest(line)))().second,
                          evaluate(newEnv, rest(rest(line)))().second)};
        };
    }
    else if constexpr(std::is_same_v<
                              decltype(function),
                              CoreInstruction<ATOM>>) {
        return [=]() {
            return std::pair{
                    newEnv,
                    Bool<atom<
                            decltype(evaluate(decltype(newEnv){}, rest(line))()
                                             .second)>>};
        };
    }
    else if constexpr(std::is_same_v<
                              decltype(function),
                              CoreInstruction<QUOTE>>) {
        return [=]() {
            return std::pair{newEnv, rest(line)};
        };
    }
    else {
        return [=]() {
            return std::pair{env, line};
        };
    }
}

auto constexpr program = List{
        CI<CONDITION>,
        List{List{CI<EQUAL>,
                  True,
                  List{CI<EQUAL>,
                       List{CI<FIRST>, List{True, False}},
                       List{CI<QUOTE>, CI<FIRST>, List{True, False}}}},
             Int<4>},
        List{True, List{CI<FIRST>, CI<REST>, Int<5>, List{Int<1>, Int<2>}}},
        List{False, NIL}};
auto constexpr a = evaluate(Environment{}, program)().second;

static_assert(a == 4);

#endif    // ELDERLISTP_INTERPRETER_HPP
