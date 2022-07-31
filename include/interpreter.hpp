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

namespace elder {

template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template<class>
struct Tag {};

template<class T, class U>
struct variant_index {};

template<class T, template<class...> class U, class... Us>
requires(std::is_same_v<T, Us> || ...) struct variant_index<T, U<Us...>> :
            std::integral_constant<
                    std::size_t,
                    U<Tag<Us>...>{Tag<T>{}}.index()> {};

template<class T, class U>
auto constexpr inline variant_index_v = variant_index<T, U>::value;

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
struct List;

using Atomic  = std::variant<NIL, bool, Data, Builtin, Function>;
using Element = std::variant<Atomic, List, Symbol>;
using ArgList = std::deque<Element>;

struct Function : std::function<Element(ArgList)> {
    friend bool
    operator==(Function const&, Function const&)
    {
        return false;
    }
};

struct List : std::deque<Element> {};

auto inline const True  = Element{Atomic{true}};
auto inline const False = Element{Atomic{false}};

struct Definitions : std::deque<std::pair<Symbol, Element>> {};

auto inline evaluate(Definitions& defs, Symbol const& symbol) -> Element
{
    auto result = std::ranges::find(
            std::rbegin(defs),
            std::rend(defs),
            symbol,
            &Definitions::value_type::first);

    if(result == std::rend(defs)) {
        throw std::runtime_error{"Unbound symbol: " + symbol};
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

auto inline front(Atomic atom) -> Element
{
    throw std::runtime_error("front/car(Atomic) should never be called");

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
                    [](auto&& element) { return front(std::move(element)); },
                    [&defs](Symbol&& symbol) {
                        return front(defs, ArgList{evaluate(defs, symbol)});
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

auto inline rest(Atomic atom) -> Element
{
    throw std::runtime_error("rest/cdr(Atomic) should never be called");

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
                    [&defs](Symbol&& symbol) {
                        return rest(defs, ArgList{evaluate(defs, symbol)});
                    },
                    [](auto&& element) {
                        return rest(std::move(element));
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

auto inline atom(Definitions& defs, ArgList args) -> Element
{
    if(args.size() != 1) {
        throw std::runtime_error(
                "atom(ArgList) should only be called w 1 argument, not"
                + std::to_string(args.size()));
    }

    return std::visit(
            overloaded{
                    [&defs](Symbol&& symbol) -> Element {
                        return atom(defs, ArgList{evaluate(defs, symbol)});
                    },
                    [](Atomic&&) -> Element { return True; },
                    [](List&&) -> Element {
                        return False;
                    }}    // namespace elder
            ,
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

    if(not std::holds_alternative<Symbol>(args[0])) {
        throw std::runtime_error{
                "def(ArgList): first argument should be a symbol"};
    }

    auto& name = std::get<Symbol>(args[0]);

    defs.push_back(std::pair{std::move(name), evaluate(defs, args[1])});

    return std::move(args[1]);
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
        if(not std::holds_alternative<Symbol>(name)) {
            throw std::runtime_error{
                    "lambda(ArgList): first argument must be a list of only Symbols"};
        }
        argNames.emplace_back(std::move(std::get<Symbol>(name)));
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

inline auto
evaluate(Definitions& defs, Atomic atom) -> Element
{
    return std::visit(
            overloaded{
                    [](auto&& atom) -> Element {
                        return Atomic{std::move(atom)};
                    },
                    [&defs](Symbol&& symbol) -> Element {
                        return evaluate(defs, symbol);
                    },
                    [](Builtin&&) -> Element {
                        throw std::runtime_error{
                                "evaluate(Builtin) should never be called"};
                        return {};
                    },
                    [](Function&&) -> Element {
                        throw std::runtime_error{
                                "evaluate(Function) should never be called"};
                        return {};
                    }},
            std::move(atom));
}

inline auto
evaluate(Definitions& defs, Builtin builtin, ArgList args) -> Element
{
    using enum Builtin;

    switch(builtin) {
    case QUOTE:
        return quote(std::move(args));
    case LIST:
        return list(std::move(args));
    case FRONT:
        return front(defs, std::move(args));
    case REST:
        return rest(defs, std::move(args));
    case JOIN:
        return join(std::move(args));
    case ATOM:
        return atom(defs, std::move(args));
    case EQUAL:
        return equal(defs, std::move(args));
    case DEF:
        return def(defs, std::move(args));
    case LAMBDA:
        return lambda(defs, std::move(args));
    case IF:
        return cond(defs, std::move(args));
    }

    return {};
}

inline auto
evaluate(Definitions& defs, List list) -> Element
{
    if(list.empty()) {
        return {};
    }

    switch(list[0].index()) {
    case variant_index_v<Symbol, Element>: {
        list[0] = evaluate(defs, std::get<Symbol>(std::move(list[0])));
        return evaluate(defs, std::move(list));
    } break;
    case variant_index_v<Atomic, Element>: {
        auto atom = std::get<Atomic>(list[0]);
        list.pop_front();

        switch(atom.index()) {
        case variant_index_v<Builtin, Atomic>: {
            return evaluate(defs, std::get<Builtin>(atom), std::move(list));
        } break;
        case variant_index_v<Function, Atomic>: {
            for(std::size_t i = 0; i < list.size(); i++) {
                list[i] = evaluate(defs, std::move(list[i]));
            }
            return std::get<Function>(atom)(static_cast<ArgList&&>(list));
        } break;
        default:
            throw std::runtime_error{
                    "evaluate(List): first element is not a function"};
        }
    } break;
    case variant_index_v<List, Element>:
        [[fallthrough]];
    default: {
        throw std::runtime_error{
                "evaluate(List): first element is not a function"};
    }
    }

    return {};
}

inline auto
evaluate(Definitions& defs, Element element) -> Element
{
    return std::visit(
            [&defs](auto&& element) {
                return evaluate(defs, std::move(element));
            },
            std::move(element));
}

}    // namespace elder

#endif    // ELDERLISP_LIST_HPP
