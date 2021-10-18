#include <iostream>
#include <variant>

#include "list.hpp"
#include "environment.hpp"
#include "interpreter.hpp"

auto constexpr program = List{CI<OUT>, C<'T'>, C<'e'>, C<'s'>, C<'t'>};
// CI<CONDITION>,
// List{List{CI<EQUAL>,
// True,
// List{CI<EQUAL>,
// List{CI<FIRST>, List{True, False}},
// List{CI<QUOTE>, CI<FIRST>, List{True, False}}}},
// Int<4>},
// List{True, List{CI<FIRST>, CI<REST>, Int<5>, List{Int<1>, Int<2>}}},
// List{False, NIL}};

int
main(int, char**)
{
    auto constexpr a = evaluate(Environment{}, program).second;
    a();
}
