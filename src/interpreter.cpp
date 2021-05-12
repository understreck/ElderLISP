#include "interpreter.hpp"

#include "meta.hpp"

#include <tuple>

namespace elderLISP {
namespace interpreter {
    template<class T>
    auto constexpr builtInIndex = meta::variantIndex<T, ast::BuiltIn>;

    template<class T>
    auto constexpr nodeIndex = meta::variantIndex<T, ast::Node>;

    auto
    interpret(ast::Node expr, Environment env)
            -> std::tuple<ast::Node, Environment>
    {
        if() {
            if(std::holds_alternative<ast::List>(expr)) {
                return {ast::NIL{}, env};
            }

            return {expr, env};
        }

        auto&& exprList = std::get<nodeIndex<ast::List>>(expr);

        return {};
    }
}    // namespace interpreter
}    // namespace elderLISP
