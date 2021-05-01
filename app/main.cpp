#include "interpreter.hpp"
#include "lexer.hpp"
#include "parser.hpp"

#include <iostream>
#include <variant>

using namespace elderLISP;
using namespace ast;

auto
pretty_print(Lambda const)
{
    std::cout << " LAMBDA ";
}

auto
pretty_print(Name const& atom)
{
    std::cout << " NAME:" << atom.name << ' ';
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
pretty_print(Equal const)
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
    std::cout << " COND ";
}

auto
pretty_print(Let const)
{
    std::cout << " LET ";
}

auto
pretty_print(True const)
{
    std::cout << " TRUE ";
}

auto
pretty_print(False const)
{
    std::cout << " FALSE ";
}

auto
pretty_print(NIL const)
{
    std::cout << " NIL ";
}

auto
pretty_print(Atomic const)
{
    std::cout << " ATOMIC ";
}

auto
pretty_print(Quote const)
{
    std::cout << " QUOTE ";
}

auto
pretty_print(ast::Atom atom) -> void
{
    auto constexpr prettyPrint = [](auto&& atom) {
        pretty_print(atom);
    };

    std::visit(prettyPrint, atom);
}

auto
pretty_print(ast::List const& list) -> void
{
    auto constexpr prettyPrint = [](auto&& token) {
        pretty_print(token);
    };

    static auto indent{0};
    indent++;
    std::cout << '\n';

    for(auto i = 0; i < indent; i++) {
        std::cout << '\t';
    }

    for(auto&& element : list) {
        std::visit(prettyPrint, element);
    }

    indent--;
}

int
main(int, char**)
{
    auto constexpr prettyPrint = [](auto&& token) {
        pretty_print(token);
    };

    std::ios_base::sync_with_stdio(false);

    auto env = interpreter::Environment{{}};
    while(true) {
        auto in = std ::string{};
        std::getline(std::cin, in);

        auto const&& tokens = tokenize(in, {});

        // std::for_each(tokens.cbegin(), tokens.cend(), [=](auto&& token) {
        // std::visit(prettyPrint, token);
        //});

        // std::cout << std::endl;

        auto const list = parser::parse(tokens);
        std::cout << list.size() << '\n';

        // std::for_each(list.cbegin(), list.cend(), [=](auto&& token) {
        // std::visit(prettyPrint, token);
        //});
        std::cout << std::endl;
        auto node = interpreter::interpret(list, env);

        std::visit(prettyPrint, node);

        std::cout << std::endl;
    }
}
