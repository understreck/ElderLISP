#include "interpreter.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "environment.hpp"

#include <iostream>
#include <variant>

using namespace elderLISP;

auto
pretty_print(ast::String const lit)
{
    std::cout << " STRING:\"" << lit.data << "\" ";
}

auto
pretty_print(ast::Integer const lit)
{
    std::cout << " INT:" << lit.data << ' ';
}

auto
pretty_print(ast::True const)
{
    std::cout << " TRUE ";
}

auto
pretty_print(ast::False const)
{
    std::cout << " FALSE ";
}

auto
pretty_print(ast::NIL const)
{
    std::cout << " NIL ";
}

auto
pretty_print(ast::Equal const)
{
    std::cout << " EQUALS ";
}

auto
pretty_print(ast::Atomic const)
{
    std::cout << " ATOMIC ";
}

auto
pretty_print(ast::First const)
{
    std::cout << " FIRST ";
}

auto
pretty_print(ast::Rest const)
{
    std::cout << " REST ";
}

auto
pretty_print(ast::Combine const)
{
    std::cout << " COMBINE ";
}

auto
pretty_print(ast::Condition const)
{
    std::cout << " COND ";
}

auto
pretty_print(ast::Let const)
{
    std::cout << " LET ";
}

auto
pretty_print(ast::Quote const)
{
    std::cout << " QUOTE ";
}

auto
pretty_print(ast::Lambda const)
{
    std::cout << " LAMBDA ";
}

auto
pretty_print(ast::Symbol const atom)
{
    std::cout << " NAME:" << atom.name << ' ';
}

auto
pretty_print(ast::Value value) -> void
{
    auto constexpr prettyPrint = [](auto&& value) {
        pretty_print(value);
    };

    std::visit(prettyPrint, value);
}

auto
pretty_print(ast::Function function) -> void
{
    auto constexpr prettyPrint = [](auto&& function) {
        pretty_print(function);
    };

    std::visit(prettyPrint, function);
}

auto
pretty_print(ast::BuiltIn atom) -> void
{
    auto constexpr prettyPrint = [](auto&& atom) {
        pretty_print(atom);
    };

    std::visit(prettyPrint, atom);
}

auto
pretty_print(ast::List list) -> void
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

    for(auto element : list) {
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

    auto env = Environment{};
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
        auto [node, newEnv] = interpreter::interpret(list, env);
        env                 = newEnv;

        std::visit(prettyPrint, node);

        std::cout << std::endl;
    }
}
