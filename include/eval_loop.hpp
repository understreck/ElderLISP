#ifndef ELDERLISP_EVAL_LOOP_HPP
#define ELDERLISP_EVAL_LOOP_HPP

#include "list.hpp"
#include "environment.hpp"
#include "interpreter.hpp"
#include "lexer.hpp"

auto consteval eval(environment auto env, atom_or_list auto line)
{
    return evaluate(env, line);
}

auto consteval eval(
        environment auto env,
        atom_or_list auto line,
        atom_or_list auto... lines)
{
    auto result = evaluate(env, line);

    if constexpr(environment<decltype(result)>) {
        return eval(result, lines...);
    }
    else {
        return eval(env, lines...);
    }
}

//template<FixedString... lines>
//auto consteval eval(environment auto env)
//{
    //return eval(env, Line<lines>...);
//}

#endif    // ELDERLISP_EVAL_LOOP_HPP
