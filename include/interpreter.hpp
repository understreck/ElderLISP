#ifndef ELDERLISP_INTERPRETER_HPP
#define ELDERLISP_INTERPRETER_HPP

#include "ast.hpp"

namespace elderLISP {

auto
interpret(ast::List program, ast::List memory = {}) -> ast::List;

}    // namespace elderLISP

#endif    // ELDERLISP_INTERPRETER_HPP
