#ifndef ELDERLISP_LEXER_HPP
#define ELDERLISP_LEXER_HPP

#include <token.hpp>

#include <string>
#include <variant>
#include <deque>

namespace elderLISP {
auto
tokenize(
        std::string::const_iterator begin,
        std::string::const_iterator end,
        std::deque<token::Token>&& tokens) -> std::deque<token::Token>;

inline auto
tokenize(
        std::string const& code,
        std::deque<token::Token> tokens) -> std::deque<token::Token> {
return tokenize(code.cbegin(), code.cend(), std::move(tokens));
}

}    // namespace elderLISP

#endif    // ELDERLISP_LEXER_HPP
