#ifndef ELDERLISP_INTERPRETER_HPP
#define ELDERLISP_INTERPRETER_HPP

#include "ast.hpp"
#include "environment.hpp"

#include <tuple>

namespace elderLISP {
namespace interpreter {
    auto
    interpret(ast::Node expr, Environment env = Environment{})
            -> std::tuple<ast::Node, Environment>;
}    // namespace interpreter
}    // namespace elderLISP

#endif    // ELDERLISP_INTERPRETER_HPP
