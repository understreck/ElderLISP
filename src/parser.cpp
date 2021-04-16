#include "parser.hpp"

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
    parse(TokenIt& position, ast::List&& ast) -> ast::List
    {
        for(; position->index() != tokenIndex<token::RParen>; position++) {
            if(position->index() == tokenIndex<token::LParen>) {
                std::cout << "list\n";
                auto&& list = parse(++position, {});

                ast.push_back(std::move(list));
            }
            else {
                std::cout << "token\n";
                ast.push_back(*position);
            }
        }

        std::cout << "end\n";
        return std::move(ast);
    }

    auto
    parse(std::deque<token::Token> const& tokens) -> ast::List
    {
        return parse(++tokens.cbegin(), ast::List{});
    }

}    // namespace parser

}    // namespace elderLISP
