#include <iostream>
#include <variant>

#include "list.hpp"
#include "environment.hpp"
#include "interpreter.hpp"
#include "lexer.hpp"

auto constexpr appendDefinition =
        parse(Line<"(define append"
                   "(lambda (left right)"
                   "  (if (eq? left ())"
                   "    right"
                   "    (cons (car left)"
                   "          (append (cdr left) right)))))">);

auto constexpr appendTwoLists = parse(Line<"(append (list 4 5) (list 6 7))">);

auto constexpr a = evaluate(Environment{}, appendDefinition);
auto constexpr b = evaluate(a, appendTwoLists);

int
main(int, char**)
{
    return length(evaluate(Environment{}, parse(Line<"(list)">)));
}
