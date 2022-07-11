#ifndef ELDERLISP_LIST_HPP
#define ELDERLISP_LIST_HPP

#include <deque>
#include <iostream>
#include <functional>
#include <type_traits>
#include <variant>
#include <string>
#include <exception>
#include <ranges>

/* Stuff that needs to be implementet
 * quote/"
 * list
 * car/front
 * cdr/back
 * cons/join
 * atom
 * eq/comp
 * def
 * lambda
 * if
 */

namespace elder {

template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

struct Data {
    friend bool
    operator==(Data const&, Data const&) = default;
};    // placeholder type for actual data
struct Symbol : std::string {};
struct NIL {
    friend bool
    operator==(NIL const&, NIL const&) = default;
};    // Signifies end of list

enum class Builtin {
    QUOTE,
    LIST,
    CAR,
    CDR,
    CONS,
    ATOM,
    EQ,
    DEF,
    LAMBDA,
    COND,

    FRONT = CAR,
    REST  = CDR,
    JOIN  = CONS,
    EQUAL = EQ,
    IF    = COND
};

struct Function;
struct Atomic;
struct List;

using Element = std::variant<List, Atomic>;
using ArgList = std::deque<Element>;

struct Function : std::function<Element(ArgList)> {
    friend bool
    operator==(Function const&, Function const&)
    {
        return false;
    }
};
struct Atomic : std::variant<NIL, bool, Symbol, Data, Builtin, Function> {};

struct List : std::deque<Element> {};

auto inline const True  = Element{Atomic{true}};
auto inline const False = Element{Atomic{false}};

struct Definitions : std::deque<std::pair<Symbol, Element>> {};

auto inline evaluate(Definitions& defs, Symbol const& s) -> Element
{
    auto result = std::ranges::find(
            std::rbegin(defs),
            std::rend(defs),
            s,
            &Definitions::value_type::first);

    if(result == std::rend(defs)) {
        throw std::runtime_error{"Unbound symbol: " + s};
    }

    return result->second;
}

auto inline evaluate(Definitions& defs, Element) -> Element;

auto inline quote(ArgList args) -> Element
{
    if(args.size() != 1) {
        throw std::runtime_error(
                "quote(ArgList) should only be called w 1 argument");
    }

    return std::move(args[0]);
}

auto inline list(ArgList args) -> Element
{
    if(args.empty()) {
        return Atomic{};
    }

    args.push_back(Atomic{});

    return List{std::move(args)};
}

auto inline front(List list) -> Element
{
    if(list.size() < 2) {
        throw std::runtime_error(
                "front/car(List) should only be called w size() >= 2, not "
                + std::to_string(list.size()));
    }

    return std::move(list[0]);
}

auto inline front(Definitions& defs, ArgList args) -> Element;

auto inline front(Definitions& defs, Atomic a) -> Element
{
    if(std::holds_alternative<Symbol>(a)) {
        return front(defs, ArgList{evaluate(defs, std::get<Symbol>(a))});
    }

    throw std::runtime_error(
            "front/car(Atomic) should never be called wo a symbol");

    return Atomic{};
}

auto inline front(Definitions& defs, ArgList args) -> Element
{
    if(args.size() != 1) {
        throw std::runtime_error(
                "front/car(ArgList) should only be called w 1 argument, not"
                + std::to_string(args.size()));
    }

    return std::visit(
            overloaded{
                    [&](Atomic&& atomic) {
                        return front(defs, std::move(atomic));
                    },
                    [](List&& list) {
                        return front(std::move(list));
                    }},
            std::move(args[0]));
}

auto inline car(Definitions& defs, ArgList args) -> Element
{
    return front(defs, std::move(args));
}

auto inline rest(List list) -> Element
{
    if(list.size() < 2) {
        throw std::runtime_error(
                "rest/cdr(List) should only be called w size() >= 2, not "
                + std::to_string(list.size()));
    }

    if(list.size() == 2) {
        return std::move(list[1]);
    }

    list.pop_front();

    return list;
}

auto inline rest(Definitions& defs, ArgList args) -> Element;

auto inline rest(Definitions& defs, Atomic a) -> Element
{
    if(std::holds_alternative<Symbol>(a)) {
        return rest(defs, ArgList{evaluate(defs, std::get<Symbol>(a))});
    }

    throw std::runtime_error(
            "rest/cdr(Atomic) should never be called wo a Symbol");

    return Atomic{};
}

auto inline rest(Definitions& defs, ArgList args) -> Element
{
    if(args.size() != 1) {
        throw std::runtime_error(
                "rest/cdr(ArgList) should only be called w 1 argument, not"
                + std::to_string(args.size()));
    }

    return std::visit(
            overloaded{
                    [&](Atomic&& atomic) {
                        return rest(defs, std::move(atomic));
                    },
                    [](List&& list) {
                        return rest(std::move(list));
                    }},
            std::move(args[0]));
}

auto inline cdr(Definitions& defs, ArgList args) -> Element
{
    return rest(defs, std::move(args));
}

auto inline join(ArgList args) -> Element
{
    if(args.size() != 2) {
        throw std::runtime_error(
                "join/cons(ArgList) should only be called w 2 args, not "
                + std::to_string(args.size()));
    }

    return List{std::move(args)};
}

auto inline cons(ArgList args) -> Element
{
    return join(std::move(args));
}

auto inline atom(Definitions& defs, ArgList args) -> Element;

auto inline atom(Definitions& defs, Atomic a) -> Element
{
    if(std::holds_alternative<Symbol>(a)) {
        return atom(defs, ArgList{evaluate(defs, std::get<Symbol>(a))});
    }

    return True;
}

auto inline atom(Definitions& defs, ArgList args) -> Element
{
    if(args.size() != 1) {
        throw std::runtime_error(
                "atom(ArgList) should only be called w 1 argument, not"
                + std::to_string(args.size()));
    }

    return std::visit(
            overloaded{
                    [&](Atomic&& a) { return atom(defs, std::move(a)); },
                    [](List&&) -> Element {
                        return False;
                    }},
            std::move(args[0]));
}

auto inline equal(Definitions& defs, ArgList args) -> Element
{
    if(args.size() != 2) {
        throw std::runtime_error(
                "equal/eq(ArgList) should only be called w 2 arguments, not"
                + std::to_string(args.size()));
    }

    return evaluate(defs, std::move(args[0]))
                           == evaluate(defs, std::move(args[1]))
                   ? True
                   : False;
}

auto inline cond(Definitions& defs, ArgList args) -> Element
{
    if(args.size() != 3) {
        throw std::runtime_error(
                "cond/if(ArgList) should only be called w 3 arguments, not"
                + std::to_string(args.size()));
    }

    auto result = evaluate(defs, std::move(args[0]));
    if(result == True) {
        return evaluate(defs, std::move(args[1]));
    }

    if(result == False) {
        return evaluate(defs, std::move(args[2]));
    }

    throw std::runtime_error(
            "cond/if(ArgList): first argument is not a conditional");

    return Atomic{};
}

auto inline def(Definitions& defs, ArgList args)
{
    if(args.size() != 2) {
        throw std::runtime_error{
                "def(ArgList) should only be called w 2 arguments, not "
                + std::to_string(args.size())};
    }

    if(not std::holds_alternative<Atomic>(args[0])
       || not std::holds_alternative<Symbol>(std::get<Atomic>(args[0]))) {
        throw std::runtime_error{
                "def(ArgList): first argument should be a symbol"};
    }

    auto& name = std::get<Symbol>(std::get<Atomic>(args[0]));

    defs.push_back(
            std::pair{std::move(name), evaluate(defs, std::move(args[1]))});
}

auto inline lambda(Definitions& defs, ArgList args) -> Element
{
    if(args.size() != 2) {
        throw std::runtime_error(
                "lambda(ArgList) should only be called w 2 arguments, not"
                + std::to_string(args.size()));
    }

    if(not std::holds_alternative<List>(args[0])) {
        throw std::runtime_error{
                "lambda(ArgList): first argument should be a list of symbols (can be empty"};
    }

    auto argNames = std::vector<Symbol>{};
    argNames.reserve(std::get<List>(args[0]).size());

    for(auto&& name : std::get<List>(args[0])) {
        if(std::holds_alternative<Atomic>(name)) {
            if(std::holds_alternative<Symbol>(std::get<Atomic>(name))) {
                argNames.push_back(std::get<Symbol>(std::get<Atomic>(name)));

                continue;
            }
        }

        throw std::runtime_error{
                "lambda(ArgList): first argument must be a list of only Symbols"};
    }

    return Atomic{Function{
            [defs, argNames = std::move(argNames), body = std::move(args[1])](
                    ArgList args) -> Element {
                if(argNames.size() != args.size()) {
                    throw std::runtime_error{
                            "lambdas must be called w correct number of arguments"};
                }

                auto defCopy = defs;
                for(std::size_t i = 0; i < argNames.size(); i++) {
                    defCopy.push_back(
                            std::pair{argNames[i], args[i]});    // args must
                                                                 // already be
                                                                 // evaluated
                                                                 // before the
                                                                 // function
                                                                 // being called
                                                                 // for this to
                                                                 // work
                }

                return evaluate(defCopy, body);
            }}};
}

}    // namespace elder

#endif    // ELDERLISP_LIST_HPP
