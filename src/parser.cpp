#include "parser.hpp"

#include "token.hpp"
#include "ast.hpp"

#include <iostream>
#include <tuple>

namespace elderLISP {

namespace parser {

    using TokenIt = std::deque<token::Token>::const_iterator;

    auto
    parse(TokenIt position, token::LParen) -> std::tuple<size_t, ast::List>{
        
    }

    auto
    parse(TokenIt position, TokenIt sentinel, ast::List ast) -> ast::List
    {
        if(position == sentinel) {
            return ast;
        }

        auto const parser =
                [position](auto&& token) -> std::tuple<size_t, ast::List> {
            return parse(position, token);
        };

        auto&& [length, astStub] = std::visit(parser, *position);

        ast.push_back(std::move(astStub));

        return parse(position + length, sentinel, std::move(ast));
    }

    auto
    parse(std::deque<token::Token> const& tokens) -> ast::List
    {
        return parse(tokens.cbegin(), tokens.cend(), ast::List{});
    }

}    // namespace parser

}    // namespace elderLISP
