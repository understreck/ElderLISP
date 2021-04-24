#ifndef ELDERLISP_PARSER_HPP
#define ELDERLISP_PARSER_HPP

#include "ast.hpp"
#include "token.hpp"

#include <deque>

namespace elderLISP {

namespace parser {
    auto
    parse(std::deque<token::Token> const& tokens) -> ast::List;
};

}    // namespace elderLISP

#endif    // ELDERLISP_PARSER_HPP
