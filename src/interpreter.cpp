#include "interpreter.hpp"

#include "environment.hpp"
#include "meta.hpp"

#include <iostream>
#include <tuple>
#include <memory>

namespace elderLISP {
namespace interpreter {
    template<class T>
    auto constexpr valueIndex = meta::variantIndex<T, ast::Value>;

    template<class T>
    auto constexpr functionIndex = meta::variantIndex<T, ast::Function>;

    template<class T>
    auto constexpr builtInIndex = meta::variantIndex<T, ast::BuiltIn>;

    template<class T>
    auto constexpr nodeIndex = meta::variantIndex<T, ast::Node>;

    auto isAtomic(ast::Node) -> bool;

    auto interpretAtom(ast::BuiltIn, Environment)
            -> std::tuple<ast::Node, Environment>;

    auto interpretList(ast::List, Environment)
            -> std::tuple<ast::Node, Environment>;

    auto
    interpret(ast::Node expr, Environment env)
            -> std::tuple<ast::Node, Environment>
    {
        if(isAtomic(expr)) {
            if(std::holds_alternative<ast::List>(expr)) {
                return {ast::NIL{}, env};
            }

            return interpretAtom(std::get<nodeIndex<ast::BuiltIn>>(expr), env);
        }

        return interpretList(std::get<nodeIndex<ast::List>>(expr), env);
    }

    auto
    interpretList(ast::List expr, Environment env)
            -> std::tuple<ast::Node, Environment>
    {

        return {ast::NIL{}, env};
    }

    auto
    isAtomic(ast::Node node) -> bool
    {
        if(std::holds_alternative<ast::List>(node)) {
            return std::get<nodeIndex<ast::List>>(node).empty();
        }

        return true;
    }

    auto
    interpretAtom(ast::BuiltIn builtIn, Environment env)
            -> std::tuple<ast::Node, Environment>
    {
        switch(builtIn.index()) {
        case builtInIndex<ast::Value>: {
            return {builtIn, env};
        } break;
        case builtInIndex<ast::Function>: {
            std::cerr << "\nERROR: Function without argument\n";

            return {ast::NIL{}, env};
        } break;
        case builtInIndex<ast::Symbol>: {
            auto [node, closedEnv] =
                    get(std::get<builtInIndex<ast::Symbol>>(builtIn), env);

            auto [result, dontCare] = interpret(node, closedEnv);

            return {result, env};
        } break;
        }

        return {ast::NIL{}, env};
    }
}    // namespace interpreter
}    // namespace elderLISP
