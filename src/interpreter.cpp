#include "interpreter.hpp"
#include "ast.hpp"
#include "interpreter-memory.hpp"

namespace elderLISP {

// auto
// interpret(ast::List)

auto
interpret(ast::List const& program, token::Token token, Memory memory) -> Memory
{
    auto const interp = [&, memory = std::move(memory)](auto&& cell) -> Memory {
        return interpret(program, cell, memory);
    };

    return std::visit(interp, token);
};

auto
interpret(ast::List const& program, ast::List const cell, Memory memory)
        -> Memory
{
    memory = push(std::move(memory));

    memory = 

    if(notGlobal) {
        memory = drop(std::move(memory));
    }
    return memory;
};

auto
interpret(ast::List program, Memory memory) -> Memory
{
    return interpret(program, 0, std::move(memory));
}

}    // namespace elderLISP
