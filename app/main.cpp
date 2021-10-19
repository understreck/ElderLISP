#include <iostream>
#include <variant>

#include "list.hpp"
#include "environment.hpp"
#include "interpreter.hpp"

auto constexpr programA = List{CI<DEFINE>, Lbl<"AyBee">, List{C<'a'>}};
//auto constexpr programB = List{CI<OUT>, Lbl<"AyBee">};
// CI<CONDITION>,
// List{List{CI<EQUAL>,
// True,
// List{CI<EQUAL>,
// List{CI<FIRST>, List{True, False}},
// List{CI<QUOTE>, CI<FIRST>, List{True, False}}}},
// Int<4>},
// List{True, List{CI<FIRST>, CI<REST>, Int<5>, List{Int<1>, Int<2>}}},
// List{False, NIL}};
auto constexpr a = evaluate(Environment{}, programA).first;
//auto constexpr b = evaluate(a, programB).second;

int
main(int, char**)
{
    //b();
}
