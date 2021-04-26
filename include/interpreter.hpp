#ifndef ELDERLISP_INTERPRETER_HPP
#define ELDERLISP_INTERPRETER_HPP

#include "ast.hpp"

#include <deque>
#include <string>
#include <unordered_map>

namespace elderLISP {

namespace interpreter {
    using Environment = std::deque<std::unordered_map<
            std::string,
            std::variant<ast::List, ast::Atom>>>;

    auto
    interpret(ast::List program, Environment environment) -> Environment;
}    // namespace interpreter
}    // namespace elderLISP

#endif    // ELDERLISP_INTERPRETER_HPP
