#ifndef ELDERLISTP_PROCEDURE_HPP
#define ELDERLISTP_PROCEDURE_HPP

#include "list.hpp"
#include "environment.hpp"

template<environment Env, list Args, atom_or_list Body>
struct Procedure<Env, Args, Body> {
    Env environment;
    Args arguments;
    Body body;
};

template<environment Env, list Args, atom_or_list Body>
Procedure(Env, Args, Body) -> Procedure<Env, Args, Body>;

auto consteval bind_argument(
        procedure auto proc,
        atom_or_list auto arg)
{
    return Procedure{
            push_kvps(
                    proc.environment,
                    std::tuple{KeyValuePair{std::get<0>(proc.arguments), arg}}),
            std::get<1>(proc.arguments),
            proc.body};
}

#endif    // ELDERLISTP_PROCEDURE_HPP
