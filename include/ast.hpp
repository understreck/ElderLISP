#ifndef ELDERLISP_AST_HPP
#define ELDERLISP_AST_HPP

#include "token.hpp"

#include <vector>
#include <memory>

namespace elderLISP {

namespace ast {
    struct List : public std::vector<std::variant<List, token::Token>> {};
}    // namespace ast

}    // namespace elderLISP

#endif    // ELDERLISP_AST_HPP
