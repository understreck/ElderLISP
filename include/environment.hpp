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
    using Storage   = std::map<std::string, std::tuple<ast::Node, SharedEnv>>;

    SharedEnv outer;
    Storage data;
};

}    // namespace elderLISP

#endif    // ELDERLISP_ENVIRONMENT_HPP
