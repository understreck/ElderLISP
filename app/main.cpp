#include <iostream>
#include <variant>

#include "list.hpp"
#include "environment.hpp"
#include "interpreter.hpp"

auto constexpr lineOne =
        ListT{CI<DEFINE>,
             Lbl<"factorial">,
             ListT{CI<LAMBDA>,
                  Lbl<"i">,
                  ListT{CI<CONDITION>,
                       ListT{ListT{CI<EQUAL>, Lbl<"i">, Int<1>}, Int<1>},
                       ListT{True,
                            ListT{CI<MUL>,
                                 Lbl<"i">,
                                 ListT{Lbl<"factorial">,
                                      ListT{CI<SUB>, Lbl<"i">, Int<1>}}}}}}};
auto constexpr lineTwo = ListT{Lbl<"factorial">, Int<5>};

auto constexpr a = evaluate(Environment{}, lineOne);
auto constexpr b = evaluate(a.first, lineTwo).second;

int
main(int, char**)
{
    static_assert(b == 120);

    std::cout << b;
}
