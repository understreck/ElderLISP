#ifndef ELDERLISP_LIST_UTIL_HPP
#define ELDERLISP_LIST_UTIL_HPP

#include "atomic-types.hpp"
#include "list.hpp"

auto consteval first(list_not_nil auto l)
{
    return l.car;
}

auto consteval rest(list_not_nil auto l)
{
    return l.cdr;
}

template<class LHS, class RHS>
auto consteval equal(LHS, RHS)
{
    return Bool<std::is_same_v<LHS, RHS>>;
}

auto consteval combine(auto lhs, auto rhs)
{
    return ListT{lhs, rhs};
}

auto consteval length(nil auto)
{
    return 0;
}

auto consteval length(auto)
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
