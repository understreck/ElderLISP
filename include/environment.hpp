#ifndef ELDERLISP_ENVIRONMENT_HPP
#define ELDERLISP_ENVIRONMENT_HPP

#include "ast.hpp"

#include <map>
#include <string>
#include <memory>
#include <tuple>

namespace elderLISP {

struct Environment {
    using SharedEnv = std::shared_ptr<Environment>;
    using Storage   = std::map<std::string, std::tuple<ast::Node, Environment>>;

    SharedEnv outer;
    Storage data;
};

using SharedEnv = std::shared_ptr<Environment>;
using Closure   = std::tuple<ast::Node, Environment>;

inline auto
get(ast::Symbol const symbol, Environment const env) -> Closure
{
    if(env.data.contains(symbol.name)) {
        return env.data.at(symbol.name);
    }

    return get(symbol, *env.outer);
}

}    // namespace elderLISP

#endif    // ELDERLISP_ENVIRONMENT_HPP
