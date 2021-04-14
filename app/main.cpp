#include "lexer.hpp"

#include <iostream>
#include <variant>

using namespace lexer;

auto
print_token(LParen const)
{
    std::cout << "LPAREN";
}

auto
print_token(RParen const)
{
    std::cout << "RPAREN";
}

auto
print_token(Atom const& atom)
{
    std::cout << ' ' << atom.name << ' ';
}

auto
print_token(Equals const)
{
    std::cout << "EQUALS";
}

auto
print_token(First const)
{
    std::cout << "FIRST";
}

auto
print_token(Rest const)
{
    std::cout << "REST";
}

auto
print_token(Combine const)
{
    std::cout << "COMBINE";
}

auto
print_token(Condition const)
{
    std::cout << "IF";
}

auto
print_token(Let const)
{
    std::cout << "LET";
}

auto
print_token(Quote const)
{
    std::cout << "QUOTE";
}

auto print_token(Whitespace)
{
    std::cout << " WHITESPACE ";
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
