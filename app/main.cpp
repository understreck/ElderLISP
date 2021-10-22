#include <iostream>
#include <variant>

#include "list.hpp"
#include "environment.hpp"
#include "interpreter.hpp"
#include "lexer.hpp"

auto constexpr Append =
        List(CI<DEFINE>,
             Lbl<"append">,
             List(CI<LAMBDA>,
                  List(Lbl<"left">, Lbl<"right">),
                  List(CI<IF>,
                       List(CI<EQUAL>, Lbl<"left">, NIL),
                       Lbl<"right">,
                       List(CI<COMBINE>,
                            List(CI<FIRST>, Lbl<"left">),
                            List(Lbl<"append">,
                                 List(List(CI<REST>, Lbl<"left">),
                                      Lbl<"right">))))));
auto constexpr lineTwo =
        List(Lbl<"append">, List(List(Int<5>, Int<6>), List(Int<7>, Int<8>)));

auto constexpr a = evaluate(Environment{}, Append);
auto constexpr b = evaluate(a.first, lineTwo).second;

int
main(int, char**)
{
}
