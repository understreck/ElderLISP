#include <iostream>
#include <variant>

#include "list.hpp"
#include "environment.hpp"
#include "interpreter.hpp"
#include "lexer.hpp"

// auto constexpr appendDefinition =
// parse(Line<"(define append"
//"(lambda (left right)"
//"  (if (eq? left ())"
//"    right"
//"    (cons (car left)"
//"          (append ((cdr left) right))))))">);

// auto constexpr appendTwoLists = parse(Line<"(append ((list 4 5) (list 6
// 7)))">);

// auto constexpr a = evaluate(Environment{}, appendDefinition);
// auto constexpr b = evaluate(a.first, appendTwoLists).second;

auto constexpr addLists = parse(Line<"(define addLists"
                                     "(lambda (a b)"
                                     "    (if"
                                     "        (eq? (eq? a ()) F)"

                                     "        (cons"
                                     "            (+ (car a) (car b))"
                                     "            (addLists (cdr a) (cdr b)))"

                                     "        (list)"

                                     ")))">);
auto constexpr prog     = parse(Line<"(addLists (list 2 5 7) (list 1 2 3))">);

auto constexpr a = evaluate(Environment{}, addLists).first;
auto constexpr b = evaluate(a, prog).second;
int
main(int, char**)
{}
