#ifndef ELDERLISP_EVAL_LOOP_HPP
#define ELDERLISP_EVAL_LOOP_HPP

#include "list.hpp"
#include "environment.hpp"
#include "interpreter.hpp"
#include "lexer.hpp"

auto consteval eval(environment auto env, c_line auto line)
{
    return evaluate(env, parse(line));
}

auto consteval eval(
        environment auto env,
        c_line auto line,
        c_line auto... lines)
{
    auto result = evaluate(env, parse(line));

    if constexpr(environment<decltype(result)>) {
        return eval(result, lines...);
    }
    else {
        return eval(env, lines...);
    }
}

template<FixedString... lines>
auto consteval eval(environment auto env)
{
    return eval(env, Line<lines>...);
}

#endif    // ELDERLISP_EVAL_LOOP_HPP
