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

auto consteval first(atom auto)
{
    return NIL;
}

auto consteval first(list_not_nil auto l)
{
    return std::get<0>(l);
}

auto consteval rest(atom auto)
{
    return NIL;
}

template<list_not_nil L>
auto consteval rest(L l)
{
    if constexpr(Length<L> >= 2) {
        return std::get<1>(l);
    }
    else {
        return NIL;
    }
}

template<atom LHS, atom RHS>
auto consteval equal(LHS, RHS)
{
    return Bool<std::is_same_v<LHS, RHS>>;
}

auto consteval equal(list_not_nil auto, atom auto)
{
    return False;
}

auto consteval equal(atom auto lhs, list_not_nil auto rhs)
{
    return equal(rhs, lhs);
}

auto consteval equal(list_not_nil auto lhs, list_not_nil auto rhs)
{
    return equal(first(lhs), first(rhs)) && equal(rest(lhs), rest(rhs));
}

auto consteval combine(atom_or_list auto lhs, atom_or_list auto rhs)
{
    if constexpr(equal(lhs, NIL)) {
        return rhs;
    }

    if constexpr(equal(rhs, NIL)) {
        return lhs;
    }
    else {
        return List{lhs, rhs};
    }
}

auto consteval append(nil auto, atom_or_list auto b)
{
    return b;
}

template<atom_or_list A>
requires (!nil<A>)
auto consteval append(A a, atom_or_list auto b)
{
    return combine(first(a), append(rest(a), b));
}

auto constexpr a = append(List{Int<5>}, NIL);

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

template<label... Args, list FBody>
auto consteval lambda(environment auto outer, List<List<Args...>, FBody> l)
{
    return std::pair{outer, combine(CI<LAMBDA>, l)};
}

template<label L>
auto consteval replace(L, atom_or_list auto replacement, L)
{
    return replacement;
}

auto consteval replace(label auto, atom_or_list auto, label auto body)
{
    return body;
}

auto consteval replace(label auto, atom_or_list auto, atom auto body)
{
    return body;
}

auto consteval replace(
        label auto name,
        atom_or_list auto replacement,
        list auto body)
{
    return combine(
            replace(name, replacement, first(body)),
            replace(name, replacement, rest(body)));
}

template<label ArgName, atom_or_list FBody>
auto consteval lambda(environment auto, List<ArgName, FBody> funcExpr)
{
    return append(CI<LAMBDA>, funcExpr);
}

template<environment Env, label ArgName, atom_or_list FBody, atom_or_list Arg>
auto consteval lambda(Env outer, List<ArgName, FBody, Arg> funcExpr)
{
    auto argName = first(funcExpr);
    auto body    = first(rest(funcExpr));
    auto arg     = evaluate(outer, rest(rest(funcExpr))).second;

    auto newBody = replace(argName, arg, body);

    return evaluate(outer, newBody).second;
}

template<label ArgName, label... ArgNs, atom_or_list FBody>
auto consteval lambda(
        environment auto,
        List<List<ArgName, ArgNs...>, FBody> funcExpr)
{
    return append(CI<LAMBDA>, funcExpr);
}

template<
        environment Env,
        label ArgName,
        label... ArgNs,
        atom_or_list FBody,
        atom_or_list Arg,
        atom_or_list... Args>
auto consteval lambda(
        Env outer,
        List<List<ArgName, ArgNs...>, FBody, List<Arg, Args...>> funcExpr)
{
    auto argName = first(first(funcExpr));
    auto body    = first(rest(funcExpr));
    auto arg     = evaluate(outer, first(rest(rest(funcExpr)))).second;

    auto newBody = replace(argName, arg, body);

    return lambda(
            outer,
            combine(rest(first(funcExpr)),
                    combine(newBody, rest(rest(rest(funcExpr))))));
}

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

    auto [newEnv, predicateResult] = evaluate(outer, predicate);
    if constexpr(std::is_same_v<decltype(predicateResult), Boolean<true>>) {
        return evaluate(newEnv, body).second;
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
        return std::pair{env, find(env, a)};
    }
    else {
        return std::pair{env, a};
    }
}

template<list Line>
requires(!std::is_same_v<Line, List<>>) auto consteval evaluate(
        environment auto env,
        atom_or_list auto function,
        Line line)
{
    if constexpr(std::is_same_v<decltype(function), CoreInstruction<FIRST>>) {
        return std::pair{env, first(evaluate(env, line).second)};
    }
    else if constexpr(std::is_same_v<
                              decltype(function),
                              CoreInstruction<REST>>) {
        return std::pair{env, rest(evaluate(env, line).second)};
    }
    else if constexpr(std::is_same_v<
                              decltype(function),
                              CoreInstruction<COMBINE>>) {
        return std::pair{
                env,
                combine(evaluate(env, first(line)).second,
                        evaluate(env, rest(line)).second)};
    }
    else if constexpr(std::is_same_v<
                              decltype(function),
                              CoreInstruction<CONDITION>>) {
        return std::pair{env, condition(env, line)};
    }
    else if constexpr(std::is_same_v<
                              decltype(function),
                              CoreInstruction<EQUAL>>) {
        return std::pair{
                env,
                equal(evaluate(env, first(line)).second,
                      evaluate(env, rest(line)).second)};
    }
    else if constexpr(std::is_same_v<
                              decltype(function),
                              CoreInstruction<ATOM>>) {
        return std::pair{
                env,
                Bool<atom<decltype(evaluate(decltype(env){}, line).second)>>};
    }
    else if constexpr(std::is_same_v<
                              decltype(function),
                              CoreInstruction<QUOTE>>) {
        return std::pair{env, line};
    }
    else if constexpr(std::is_same_v<
                              decltype(function),
                              CoreInstruction<DEFINE>>) {
        return define(
                env,
                evaluate(env, first(line)).second,
                evaluate(env, rest(line)).second);
    }
    else if constexpr(std::is_same_v<
                              decltype(function),
                              CoreInstruction<LAMBDA>>) {
        return std::pair{env, lambda(env, line)};
    }
    else if constexpr(std::is_same_v<
                              decltype(function),
                              CoreInstruction<MUL>>) {
        return std::pair{
                env,
                Int<decltype(evaluate(env, first(line)).second)::
                            value* decltype(evaluate(env, rest(line))
                                                    .second)::value>};
    }
    else if constexpr(std::is_same_v<
                              decltype(function),
                              CoreInstruction<SUB>>) {
        return std::pair{
                env,
                Int<decltype(evaluate(env, first(line)).second)::value
                    - decltype(evaluate(env, rest(line)).second)::value>};
    }
    else {
        return std::pair{env, line};
    }
}

template<list Line>
requires(!std::is_same_v<Line, List<>>) auto consteval evaluate(
        environment auto env,
        Line line)
{
    auto result = evaluate(env, first(line));

    if constexpr(
            core_instruction<
                    decltype(result.second)> || core_instruction<decltype(first(result.second))>) {
        if constexpr(Length<Line>::value == 2) {
            if constexpr(list<decltype(result.second)>) {
                auto newLine =
                        []<atom_or_list... As>(List<As...>, atom_or_list auto b)
                {
                    return List{As{}..., b};
                }
                (result.second, rest(line));

                return evaluate(result.first, first(newLine), rest(newLine));
            }
            else {
                return evaluate(result.first, result.second, rest(line));
            }
        }
        else if constexpr(Length<Line>::value > 2) {
            auto newLine = append(result.second, rest(line));
            return evaluate(result.first, first(newLine), rest(newLine));
        }
        else {
            return result;
        }
    }
    else {
        return std::pair{env, line};
    }
}

auto constexpr ev = [](auto... args) {
    return evaluate(args...);
};
#endif    // ELDERLISTP_INTERPRETER_HPP
