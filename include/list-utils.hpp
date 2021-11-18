#ifndef ELDERLISP_LIST_UTIL_HPP
#define ELDERLISP_LIST_UTIL_HPP

#include "atomic-types.hpp"
#include "list.hpp"

auto consteval first(list_not_nil auto l)
{
    return std::get<0>(l);
}

auto consteval rest(list auto l)
{
    return std::get<1>(l);
}

template<atom_or_list LHS, atom_or_list RHS>
auto consteval equal(LHS, RHS)
{
    return Bool<std::is_same_v<LHS, RHS>>;
}

auto consteval combine(atom auto lhs, atom_not_nil auto rhs)
{
    return ListT{lhs, rhs};
}

auto consteval combine(atom_or_list auto lhs, nil auto)
{
    return List(lhs);
}

template<size_t ElementsLeft>
requires(ElementsLeft == 1) auto consteval unroll_right(
        atom_or_list auto lhs,
        atom_or_list auto lastElement,
        atom_or_list auto... elements)
{
    return ListT{lhs, elements..., lastElement};
}

template<size_t ElementsLeft>
requires(ElementsLeft >= 2) auto consteval unroll_right(
        atom_or_list auto lhs,
        list_not_nil auto restOfList,
        atom_or_list auto... elements)
{
    return unroll_right<ElementsLeft - 1>(
            lhs,
            rest(restOfList),
            elements...,
            first(restOfList));
}

template<atom_or_list... Elements>
requires(sizeof...(Elements) >= 2) auto consteval combine(
        atom_or_list auto lhs,
        ListT<Elements...> rhs)
{
    return unroll_right<sizeof...(Elements) - 1>(lhs, rest(rhs), first(rhs));
}

auto consteval length(nil auto)
{
    return 0;
}

auto consteval length(atom_not_nil auto)
{
    return 1;
}

auto consteval length(list_not_nil auto l)
{
    return 1 + length(rest(l));
}

auto consteval append(nil auto, list auto b)
{
    return b;
}

auto consteval append(list_not_nil auto a, list auto b)
{
    return combine(first(a), append(rest(a), b));
}

#endif    // ELDERLISP_LIST_UTIL_HPP
