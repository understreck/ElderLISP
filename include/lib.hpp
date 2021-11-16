#ifndef ELDERLISP_LIB_HPP
#define ELDERLISP_LIB_HPP

#include "eval_loop.hpp"

namespace lisp {
auto constexpr nil = eval<"(define nil (list))">(Environment{});
auto constexpr map =
        eval<"(define map"
             "(lambda (f l)"
             "    (if (eq? nil l)"
             "        nil"
             "        (cons"
             "            (f (car l))"
             "            (map f (cdr l))))))">(nil);
auto constexpr foldr =
        eval<"(define foldr"
             "(lambda (f last l)"
             "(if (eq? l nil)"
             "    last"
             "    (f (car l) (reduce f last (cdr l))))))">(map);

auto constexpr lib = foldr;
}    // namespace lisp

#endif    // ELDERLISP_EVAL_LOOP_HPP
