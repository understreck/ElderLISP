#ifndef ELDERLISP_LIST_HPP
#define ELDERLISP_LIST_HPP

#include <deque>
#include <variant>
#include <string>

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
    BACK  = CDR,
    JOIN  = CONS,
    COMP  = EQ
};

struct Element : std::variant<NIL, Data, Builtin> {};
struct List;
using return_t = std::variant<List, Element>;
struct List : std::deque<return_t> {};

}    // namespace elder

#endif    // ELDERLISP_LIST_HPP
