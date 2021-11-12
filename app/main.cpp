#include <iostream>
#include <variant>

#include "eval_loop.hpp"

auto constexpr a =
        eval<"(define append"
             "(lambda (left right)"
             "  (if (eq? left ())"
             "    right"
             "    (cons (car left)"
             "          (append (cdr left) right)))))",
             "(append (list 4 5) (list 6 7))">;

int
main(int, char**)
{
}
