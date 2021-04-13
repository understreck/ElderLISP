#include "lexer.hpp"

#include <iostream>
#include <variant>

using namespace lexer;

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

auto
print_token(Equals const)
{
    std::cout << " eq ";
}

auto
print_token(First const)
{
    std::cout << " first ";
}

auto
print_token(Rest const)
{
    std::cout << " rest ";
}

auto
print_token(Combine const)
{
    std::cout << " comb ";
}

auto
print_token(Condition const)
{
    std::cout << " if ";
}

auto
print_token(Let const)
{
    std::cout << " let ";
}

auto
print_token(Quote const)
{
    std::cout << '\"';
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
