#ifndef ELDERLISP_EXERCISES_ONE_HPP
#define ELDERLISP_EXERCISES_ONE_HPP

#include "lib.hpp"

auto constexpr oneTwo =
        eval(lisp::lib, parse(Line<"(+ (/ (foldr + (list 3 7 9) 0) 3) (/ 2 3))">));
//"(foldr + (list 5 4 (- 2 (- 3 (+ 6 1)))) 0)"
//"(foldr * (list 3 (- 6 2) (- 2 7)) 1))">));

#endif    // ELDERLISP_EXERCISES_ONE_HPP
