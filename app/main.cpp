#include "lexer.hpp"

#include <iostream>
#include <variant>

auto
print_token(NIL const)
{
    std::cout << " ::NIL" << std::endl;
}

auto
print_token(LPeren const)
{
    std::cout << '(';
}

auto
print_token(RPeren const)
{
    std::cout << ')';
}

auto
print_token(Atom const& atom)
{
    std::cout << ' ' << atom.name << ' ';
}

int
main(int, char**)
{
    auto constexpr printToken = [](auto&& token) {
        print_token(token);
    };

    std::ios_base::sync_with_stdio(false);

    while(true) {
        auto in = std ::string{};
        std::getline(std::cin, in);

        auto const&& ast = tokenize(in.cbegin(), in.cend(), {});

        std::for_each(ast.cbegin(), ast.cend(), [=](auto&& token) {
            std::visit(printToken, token);
        });

        std::cout << std::endl;
    }
}
