#include <iostream>
#include <variant>

#include "list.hpp"
#include "environment.hpp"
#include "interpreter.hpp"

auto constexpr lineOne =
        List(CI<DEFINE>,
             Lbl<"factorial">,
             List(CI<LAMBDA>,
                  List(Lbl<"i">),
                  List(CI<IF>,
                       List(CI<EQUAL>, Lbl<"i">, Int<1>),
                       Int<1>,
                       List(CI<MUL>,
                            Lbl<"i">,
                            List(Lbl<"factorial">,
                                 List(List(CI<SUB>, Lbl<"i">, Int<1>)))))));
auto constexpr lineTwo = List(Lbl<"factorial">, List(Int<5>));

auto constexpr a = evaluate(Environment{}, lineOne);
auto constexpr b = evaluate(a.first, lineTwo).second;

int
main(int, char**)
{
    static_assert(b == 120);

    std::cout << b;
}
