#include <iostream>
#include <variant>

#include "list.hpp"
#include "environment.hpp"
#include "interpreter.hpp"

auto constexpr lineOne =
        List{CI<DEFINE>,
             Lbl<"factorial">,
             List{CI<LAMBDA>,
                  Lbl<"i">,
                  List{CI<CONDITION>,
                       List{List{CI<EQUAL>, Lbl<"i">, Int<1>}, Int<1>},
                       List{True,
                            List{CI<MUL>,
                                 Lbl<"i">,
                                 List{Lbl<"factorial">,
                                      List{CI<SUB>, Lbl<"i">, Int<1>}}}}}}};
auto constexpr lineTwo = List{Lbl<"factorial">, Int<5>};

auto constexpr a = evaluate(Environment{}, lineOne);
auto constexpr b = evaluate(a.first, lineTwo).second;

int
main(int, char**)
{
    static_assert(b == 120);
    std::cout << b;
}
