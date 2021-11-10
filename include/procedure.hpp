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

template<environment Env, atom_or_list Body>
auto consteval bind_argument(Procedure<Env, ListT<>, Body>, atom_or_list auto)
{
    static_assert(std::is_same_v<Env, void>, "No unbound arguments left");
}

template<environment Env, class argName, class rest, atom_or_list Body>
auto consteval bind_argument(
        Procedure<Env, ListT<argName, rest>, Body> proc,
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
