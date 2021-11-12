#ifndef ELDERLISP_EVAL_LOOP_HPP
#define ELDERLISP_EVAL_LOOP_HPP

#include "list.hpp"
#include "environment.hpp"
#include "interpreter.hpp"
#include "lexer.hpp"

auto consteval repl(environment auto env, c_line auto line)
{
    return evaluate(env, parse(line));
}

auto consteval repl(
        environment auto env,
        c_line auto line,
        c_line auto... lines)
{
    auto result = evaluate(env, parse(line));

    if constexpr(environment<decltype(result)>) {
        return repl(result, lines...);
    }
    else {
        return repl(env, lines...);
    }
}

auto consteval repl(c_line auto... lines)
{
    return repl(Environment{}, lines...);
}

template<FixedString... lines>
auto constexpr eval = repl(Line<lines>...);

#endif //ELDERLISP_EVAL_LOOP_HPP
