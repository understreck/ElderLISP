#ifndef ELDERLISP_INTERPRETER_HPP
#define ELDERLISP_INTERPRETER_HPP

#include "ast.hpp"

#include <vector>
#include <string>
#include <unordered_map>

namespace elderLISP {

namespace interpreter {
    using Environment = std::vector<std::unordered_map<std::string, ast::Node>>;

    struct Scope {
        Scope(Environment& env) : environment{env}
        {
            environment.push_back({});
        }

        Scope(Scope const&) = delete;
        Scope(Scope&&)      = delete;

        auto operator=(Scope) = delete;
        auto
        operator=(Scope&&) = delete;

        ~Scope()
        {
            environment.pop_back();
        }

        Environment& environment;
    };

    auto
    interpret(ast::List expression, Environment& environment)
            -> ast::Node;
}    // namespace interpreter
}    // namespace elderLISP

#endif    // ELDERLISP_INTERPRETER_HPP
