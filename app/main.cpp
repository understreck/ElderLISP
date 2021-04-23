#include "lexer.hpp"

#include <iostream>
#include <variant>

using namespace elderLISP;
using namespace token;

auto
pretty_print(LParen const)
{
    std::cout << "LPAREN ";
}

auto
pretty_print(RParen const)
{
    std::cout << " RPAREN";
}

auto
pretty_print(Lambda const)
{
    std::cout << " LAMBDA ";
}

auto
pretty_print(Name const& atom)
{
    std::cout << ' ' << atom.name << ' ';
}

auto
pretty_print(StringLiteral const& lit)
{
    std::cout << " STRING:\"" << lit.data << "\" ";
}

auto
pretty_print(IntegerLiteral const& lit)
{
    std::cout << " INT:" << lit.data << ' ';
}

auto
pretty_print(Equals const)
{
    std::cout << " EQUALS ";
}

auto
pretty_print(First const)
{
    std::cout << " FIRST ";
}

auto
pretty_print(Rest const)
{
    std::cout << " REST ";
}

auto
pretty_print(Combine const)
{
    std::cout << " COMBINE ";
}

auto
pretty_print(Condition const)
{
    std::cout << " IF ";
}

auto
pretty_print(Let const)
{
    std::cout << " LET ";
}

auto
pretty_print(Quote const)
{
    std::cout << " QUOTE ";
}

// auto
// pretty_print(ast::List const& list) -> void
//{
// auto constexpr prettyPrint = [](auto&& token) {
// pretty_print(token);
//};

// std::cout << '\n';
// for(auto&& element : list) {
// std::visit(prettyPrint, element);
//}
//}

int
main(int, char**)
{
    auto constexpr prettyPrint = [](auto&& token) {
        pretty_print(token);
    };

    std::ios_base::sync_with_stdio(false);

    while(true) {
        auto in = std ::string{};
        std::getline(std::cin, in);

        auto const&& tokens = tokenize(in, {});

        std::for_each(tokens.cbegin(), tokens.cend(), [=](auto&& token) {
            std::visit(prettyPrint, token);
        });

        std::cout << std::endl;

        // auto const list = parser::parse(tokens);
        // std::cout << list.size() << '\n';

        // std::for_each(list.cbegin(), list.cend(), [=](auto&& token) {
        // std::visit(prettyPrint, token);
        //});

        std::cout << std::endl;
    }
}
