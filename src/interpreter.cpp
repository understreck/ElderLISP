#include "interpreter.hpp"
#include "ast.hpp"

namespace elderLISP {
namespace interpreter {



    auto
    interpret(ast::List program, Environment environment) -> Environment
    {
        environment.push_back({});

        return std::visit(interpreter, program;
    }
}    // namespace interpreter
}    // namespace elderLISP
