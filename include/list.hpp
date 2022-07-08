#ifndef ELDERLISP_LIST_HPP
#define ELDERLISP_LIST_HPP

#include <deque>
#include <iostream>
#include <functional>
#include <variant>
#include <string>
#include <exception>

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
struct Data {};    // placeholder type for actual data
struct Symbol {
    std::string name;

    auto constexpr
    operator*() const noexcept -> std::string const&
    {
        return name;
    }
};
struct NIL {};    // Signifies end of list

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
    IF,

    FRONT = CAR,
    REST  = CDR,
    JOIN  = CONS,
    COMP  = EQ
};

struct Function;
struct Atomic;
struct List;
using Element = std::variant<List, Atomic>;

struct Function : std::function<Element(Element)> {};
struct Atomic : std::variant<NIL, Data, Builtin, Function> {};
struct List : std::deque<Element> {};

using ArgList = std::deque<Element>;

auto inline evaluate(Element) -> Element;

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

auto inline front(Atomic) -> Element
{
    throw std::runtime_error("front/car(Atomic) should never be called");

    return Atomic{};
}

auto inline front(ArgList args) -> Element
{
    if(args.size() != 1) {
        throw std::runtime_error(
                "front/car(ArgList) should only be called w 1 argument, not"
                + std::to_string(args.size()));
    }

    return std::visit(
            [](auto&& e) { return front(std::move(e)); },
            std::move(args[0]));
}

auto inline car(ArgList args) -> Element
{
    return front(std::move(args));
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

auto inline rest(Atomic) -> Element
{
    throw std::runtime_error("rest/cdr(Atomic) should never be called");

    return Atomic{};
}

auto inline rest(ArgList args) -> Element
{
    if(args.size() != 1) {
        throw std::runtime_error(
                "rest/cdr(ArgList) should only be called w 1 argument, not"
                + std::to_string(args.size()));
    }

    return std::visit(
            [](auto&& e) { return rest(std::move(e)); },
            std::move(args[0]));
}

auto inline cdr(ArgList args) -> Element
{
    return rest(std::move(args));
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

auto inline atom(List list)
{}

}    // namespace elder

#endif    // ELDERLISP_LIST_HPP
