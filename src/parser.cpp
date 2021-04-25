#include "parser.hpp"

#include "token.hpp"
#include "ast.hpp"

#include <iostream>
#include <tuple>

namespace elderLISP {

namespace parser {

    template<class T, class... Us>
    struct Index {};

    template<class T, class... Us>
    struct Index<T, T, Us...> {
        static auto constexpr value = 0ul;
    };

    template<class T, class U, class... Vs>
    struct Index<T, U, Vs...> {
        static auto constexpr value = 1ul + Index<T, Vs...>::value;
    };

    template<class T, class... Us>
    auto constexpr index = Index<T, Us...>::value;

    template<class T, class U>
    struct VariantIndex {};

    template<class T, class... Us>
    struct VariantIndex<T, std::variant<Us...>> {
        static auto constexpr value = index<T, Us...>;
    };

    template<class T, class Variant>
    auto constexpr variantIndex = VariantIndex<T, Variant>::value;

    template<class T>
    auto constexpr tokenIndex = variantIndex<T, token::Token>;

    using TokenIt = std::deque<token::Token>::const_iterator;

    auto
    parse(TokenIt position, ast::List ast) -> std::tuple<TokenIt, ast::List>
    {
        for(; !std::holds_alternative<token::RParen>(*position); ++position) {
            switch(position->index()) {
            case tokenIndex<token::LParen>: {
                auto&& [newPos, node] = parse(++position, {});

                position = newPos;
                ast.push_back(std::move(node));
            } break;

            case tokenIndex<token::Combine>: {
                ast.push_back(ast::Combine{});
            } break;

            case tokenIndex<token::Condition>: {
                ast.push_back(ast::Condition{});
            } break;

            case tokenIndex<token::StringLiteral>: {
                auto constexpr stringLitIndex =
                        tokenIndex<token::StringLiteral>;

                ast.push_back(ast::StringLiteral{
                        std::get<stringLitIndex>(*position).data});
            } break;

            case tokenIndex<token::IntegerLiteral>: {
                auto constexpr integerLitIndex =
                        tokenIndex<token::IntegerLiteral>;

                ast.push_back(ast::IntegerLiteral{
                        std::get<integerLitIndex>(*position).data});
            } break;

            case tokenIndex<token::Name>: {
                auto constexpr nameIndex = tokenIndex<token::Name>;

                ast.push_back(ast::Name{std::get<nameIndex>(*position).name});
            } break;

            case tokenIndex<token::Equal>: {
                ast.push_back(ast::Equal{});
            } break;

            case tokenIndex<token::First>: {
                ast.push_back(ast::First{});
            } break;

            case tokenIndex<token::Rest>: {
                ast.push_back(ast::Rest{});
            } break;

            case tokenIndex<token::Lambda>: {
                ast.push_back(ast::Lambda{});
            } break;

            case tokenIndex<token::Let>: {
                ast.push_back(ast::Let{});
            } break;

            case tokenIndex<token::Quote>: {
                ast.push_back(ast::Quote{});
            } break;
            }
        }

        return {position, std::move(ast)};
    }

    auto
    parse(std::deque<token::Token> const& tokens) -> ast::List
    {
        return std::get<1>(parse(++tokens.cbegin(), ast::List{}));
    }

}    // namespace parser

}    // namespace elderLISP
