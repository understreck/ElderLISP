#ifndef ELDERLISTP_INTERPRETER_HPP
#define ELDERLISTP_INTERPRETER_HPP

#include <type_traits>

#include "list.hpp"
#include "list-utils.hpp"
#include "environment.hpp"
#include "procedure.hpp"

auto consteval evaluate(environment auto, atom_or_list auto...);

auto consteval quote(atom_or_list auto aol)
{
    return aol;
}

auto consteval condition(
        environment auto outer,
        boolean auto predicate,
        atom_or_list auto ifTrue,
        atom_or_list auto ifFalse)
{
    if constexpr(predicate) {
        return evaluate(outer, ifTrue);
    }
    else {
        return evaluate(outer, ifFalse);
    }
};

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
        return first(evaluate(env, first(args)));
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
        return rest(evaluate(env, first(args)));
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
        return combine(
                evaluate(env, first(args)),
                evaluate(env, first(rest(args))));
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
    if constexpr(!nil<decltype(rest(rest(rest(args))))>) {
        static_assert(
                std::is_same_v<decltype(args), void>,
                "Ifs arguments need to be NIL terminated");
    }
    else {
        auto predicate = first(args);
        auto ifTrue    = first(rest(args));

        auto ifFalse = first(rest(rest(args)));

        return condition(env, evaluate(env, predicate), ifTrue, ifFalse);
    }
}

auto consteval evaluate(
        environment auto env,
        CoreInstruction<EQUAL>,
        atom_or_list auto args)
{
    if constexpr(length(args) != 2) {
        static_assert(
                std::is_same_v<decltype(args), void>,
                "Incorrect amount of arguments for Equal");
    }
    if constexpr(!nil<decltype(rest(rest(args)))>) {
        static_assert(
                std::is_same_v<decltype(args), void>,
                "Equals arguments need to be NIL terminated");
    }
    else {
        return equal(
                evaluate(env, first(args)),
                evaluate(env, first(rest(args))));
    }
}

auto consteval evaluate(
        environment auto env,
        CoreInstruction<ATOM>,
        atom_or_list auto args)
{
    if constexpr(length(args) != 1) {
        static_assert(
                std::is_same_v<decltype(args), void>,
                "Incorrect amount of arguments for Atom");
    }
    else if constexpr(!nil<decltype(rest(args))>) {
        static_assert(
                std::is_same_v<decltype(args), void>,
                "Atoms argument needs to be NIL terminated");
    }
    else {
        return Bool<atom<decltype(evaluate(env, first(args)))>>;
    }
}

auto consteval evaluate(
        environment auto /*env*/,
        CoreInstruction<QUOTE>,
        atom_or_list auto args)
{
    if constexpr(length(args) != 1) {
        static_assert(
                std::is_same_v<decltype(args), void>,
                "Incorrect amount of arguments for Quote");
    }
    else {
        return args;
    }
}

auto consteval evaluate(
        environment auto env,
        CoreInstruction<LAMBDA>,
        atom_or_list auto args)
{
    if constexpr(length(args) != 2) {
        static_assert(
                std::is_same_v<decltype(args), void>,
                "Incorrect amount of arguments for Lambda");
    }
    else if constexpr(!nil<decltype(rest(rest(args)))>) {
        static_assert(
                std::is_same_v<decltype(args), void>,
                "Lambdas arguments needs to be NIL terminated");
    }
    else {
        return Procedure{env, first(args), first(rest(args))};
    }
}

auto consteval evaluate(
        environment auto env,
        CoreInstruction<DEFINE>,
        atom_or_list auto args)
{
    if constexpr(length(args) != 2) {
        static_assert(
                std::is_same_v<decltype(args), void>,
                "Incorrect amount of arguments for Define");
    }
    else if constexpr(!nil<decltype(rest(rest(args)))>) {
        static_assert(
                std::is_same_v<decltype(args), void>,
                "Defines argument needs to be NIL terminated");
    }
    else {
        auto name = first(args);
        static_assert(
                label<decltype(name)>,
                "Defines first argument should be a label");

        auto value = first(rest(args));
        if constexpr(is_atom(value)) {
            return push_kvps(env, std::tuple{KeyValuePair{name, value}});
        }
        else {
            return push_kvps(
                    env,
                    std::tuple{KeyValuePair{name, evaluate(env, value)}});
        }
    }
}

auto consteval make_list(environment auto, atom auto arg)
{
    static_assert(
            nil<decltype(arg)>,
            "List either takes NIL or a NIL terminated series of arguments");

    return NIL;
}

auto consteval make_list(environment auto env, list_not_nil auto args)
{
    auto value = first(args);

    if constexpr(is_atom(value)) {
        if constexpr(label<decltype(value)>) {
            return combine(find(env, value), make_list(env, rest(args)));
        }
        else {
            return combine(value, make_list(env, rest(args)));
        }
    }
    else {
        return combine(evaluate(env, value), make_list(env, rest(args)));
    }
}

auto consteval evaluate(
        environment auto env,
        CoreInstruction<LIST>,
        atom_or_list auto args)
{
    return make_list(env, args);
}

auto consteval evaluate(environment auto env, atom auto arg)
{
    if constexpr(is_atom(arg)) {
        if constexpr(label<decltype(arg)>) {
            return find(env, arg);
        }
        else {
            return arg;
        }
    }
    else {
        return evaluate(env, arg);
    }
}

template<list List>
auto consteval evaluate(
        environment auto env,
        core_instruction auto function,
        List args)
{
    if constexpr(equal(function, CI<MUL>)) {
        return Int<
                evaluate(env, first(args)) * evaluate(env, first(rest(args)))>;
    }
    else if constexpr(equal(function, CI<SUB>)) {
        return Int<
                evaluate(env, first(args)) - evaluate(env, first(rest(args)))>;
    }
    else if constexpr(equal(function, CI<ADD>)) {
        return Int<
                evaluate(env, first(args)) + evaluate(env, first(rest(args)))>;
    }
    else if constexpr(equal(function, CI<DIV>)) {
        return Int<
                evaluate(env, first(args)) / evaluate(env, first(rest(args)))>;
    }
    else if constexpr(equal(function, CI<MOD>)) {
        return Int<
                evaluate(env, first(args)) % evaluate(env, first(rest(args)))>;
    }
}

auto consteval evaluate(
        environment auto env,
        procedure auto proc,
        atom auto arg)
{
    static_assert(
            nil<decltype(arg)>,
            "A procedure either takes NIL or"
            "NIL terminated series of arguments");

    if constexpr(length(proc.arguments) == 0) {
        return evaluate(
                push_kvps(push_scope(env), proc.environment.kvps),
                proc.body);
    }
    else {
        return proc;
    };
}
auto consteval evaluate(
        environment auto env,
        procedure auto proc,
        list_not_nil auto args)
{
    auto value = first(args);

    return evaluate(env, bind_argument(proc, evaluate(env, value)), rest(args));
}

auto consteval find_if_label(environment auto, auto a)
{
    return a;
}

auto consteval find_if_label(environment auto env, label auto l)
{
    return find(env, l);
}

auto consteval evaluate(environment auto env, list_not_nil auto line)
{
    auto f = find_if_label(env, first(line));

    if constexpr(is_core_instruction(f)) {
        return evaluate(env, f, rest(line));
    }
    else if constexpr(procedure<decltype(f)>) {
        return evaluate(env, f, rest(line));
    }
    else {
        static_assert(
                std::is_same_v<decltype(f), void>,
                "First element of list is not a core instruction or a procedure");
        return;
    }
}
#endif    // ELDERLISTP_INTERPRETER_HPP
