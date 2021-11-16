#ifndef ELDERLISP_LIB_HPP
#define ELDERLISP_LIB_HPP

#include "eval_loop.hpp"

namespace lisp {
auto constexpr nil = parse(Line<"(define nil (list))">);
auto constexpr map = parse(Line<"(define map"
                                "(lambda (f l)"
                                "    (if (= nil l)"
                                "        nil"
                                "        (cons"
                                "            (f (car l))"
                                "            (map f (cdr l))))))">);
auto constexpr foldr =
        parse(Line<"(define foldr"
                   "(lambda (f last l)"
                   "(if (= l nil)"
                   "    last"
                   "    (f (car l) (reduce f last (cdr l))))))">);

auto constexpr or_ = parse(Line<"(define or"
                                "(lambda (a b)"
                                "(if (= a T)"
                                "    T"
                                "    (= b T))))">);

auto constexpr and_ = parse(Line<"(define and"
                                 "(lambda (a b)"
                                 "(if (= a T)"
                                 "    (= b T)"
                                 "    F )))">);

auto constexpr not_ = parse(Line<"(define not"
                                 "(lambda (a) (= a F)))">);

auto constexpr lessOrEqual =
        parse(Line<"(define <="
                   "(lambda (a b) (or (< a b) (= a b))))">);

auto constexpr greaterOrEqual =
        parse(Line<"(define >="
                   "(lambda (a b) (or (> a b) (= a b))))">);

auto constexpr lib =
        eval(Environment{},
             nil,
             map,
             foldr,
             or_,
             and_,
             not_,
             lessOrEqual,
             greaterOrEqual);
}    // namespace lisp

#endif    // ELDERLISP_EVAL_LOOP_HPP
