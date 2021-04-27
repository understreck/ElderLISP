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
            ast::Node>>;

    auto
    interpret(ast::List const& expression, Environment& environment) -> ast::Node;
}    // namespace interpreter
}    // namespace elderLISP

#endif    // ELDERLISP_INTERPRETER_HPP
