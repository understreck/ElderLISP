#ifndef ELDERLISP_INTERPRETER_HPP
#define ELDERLISP_INTERPRETER_HPP

#include "ast.hpp"
#include "interpreter-memory.hpp"

namespace elderLISP {

auto
interpret(ast::List program, Memory memory = {}) -> Memory;

}    // namespace elderLISP

#endif    // ELDERLISP_INTERPRETER_HPP
