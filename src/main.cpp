#include "interpreter.hpp"
#include "lexer.hpp"

auto
main() -> int
{
    using namespace elder;
    auto a = lex::next_token({"   45   "}).first;
    std::cout << std::get<int>(a);
}
