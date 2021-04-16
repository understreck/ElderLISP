#ifndef ELDERLISP_VALUE_HPP
#define ELDERLISP_VALUE_HPP

#include "token.hpp"

#include <string>
#include <variant>

namespace elderLISP {

namespace value {

    using Value = std::variant<long long, std::string, token::Token>;

}

}    // namespace elderLISP

#endif    // ELDERLISP_VALUE_HPP
