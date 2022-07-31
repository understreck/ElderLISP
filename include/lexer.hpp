#ifndef ELDER_LEXER_HPP
#define ELDER_LEXER_HPP

#include "interpreter.hpp"

#include <ctre.hpp>

#include <string>
#include <string_view>
#include <span>
#include <ranges>
#include <tuple>
#include <charconv>

namespace elder {
namespace lex {
    using whitespace  = decltype(ctre::match<"\\s*">);
    using lperen      = decltype(ctre::match<"[(]">);
    using rperen      = decltype(ctre::match<"[)]">);
    using quote       = decltype(ctre::match<"quote|\'">);
    using list        = decltype(ctre::match<"list">);
    using front       = decltype(ctre::match<"front|car">);
    using rest        = decltype(ctre::match<"rest|cdr">);
    using join        = decltype(ctre::match<"join|cons">);
    using atom        = decltype(ctre::match<"atom">);
    using equal       = decltype(ctre::match<"=">);
    using define      = decltype(ctre::match<"def">);
    using lambda      = decltype(ctre::match<"y|lambda">);
    using conditional = decltype(ctre::match<"\\?">);
    using integer     = decltype(ctre::match<"[0-9]+">);
    using string      = decltype(ctre::match<"\"(.*)\"">);
    using symbol      = decltype(ctre::match<"[^\\s]+">);

    enum class Token {
        WHITESPACE,
        LPEREN,
        RPEREN,
        QUOTE,
        LIST,
        FRONT,
        REST,
        JOIN,
        ATOM,
        EQUAL,
        DEFINE,
        LAMBDA,
        COND
    };
    using Data = std::variant<int, std::string, terp::Symbol, Token>;

    inline auto
    next_token(std::string_view code) -> std::pair<Data, int>
    {
        auto trimmed = code.substr(whitespace::starts_with(code).size());

        if(auto token = lperen::starts_with(trimmed); token) {
            return {Token::LPEREN, token.size()};
        }
        if(auto token = rperen::starts_with(trimmed); token) {
            return {Token::RPEREN, token.size()};
        }
        if(auto token = quote::starts_with(trimmed); token) {
            return {Token::QUOTE, token.size()};
        }
        if(auto token = list::starts_with(trimmed); token) {
            return {Token::LIST, token.size()};
        }
        if(auto token = front::starts_with(trimmed); token) {
            return {Token::FRONT, token.size()};
        }
        if(auto token = rest::starts_with(trimmed); token) {
            return {Token::REST, token.size()};
        }
        if(auto token = join::starts_with(trimmed); token) {
            return {Token::JOIN, token.size()};
        }
        if(auto token = atom::starts_with(trimmed); token) {
            return {Token::ATOM, token.size()};
        }
        if(auto token = equal::starts_with(trimmed); token) {
            return {Token::EQUAL, token.size()};
        }
        if(auto token = define::starts_with(trimmed); token) {
            return {Token::DEFINE, token.size()};
        }
        if(auto token = lambda::starts_with(trimmed); token) {
            return {Token::LAMBDA, token.size()};
        }
        if(auto token = conditional::starts_with(trimmed); token) {
            return {Token::COND, token.size()};
        }
        if(auto token = integer::starts_with(trimmed); token) {
            int value      = 0;
            auto [capture] = token;
            std::ignore =
                    std::from_chars(capture.begin(), capture.end(), value);

            return {value, token.size()};
        }
        if(auto token = string::starts_with(trimmed); token) {
            return {token.get<1>().to_string(), token.size()};
        }
        if(auto token = symbol::starts_with(trimmed); token) {
            return {terp::Symbol{token.to_string()}, token.size()};
        }

        throw std::runtime_error(
                std::string{
                        "next_token(std::string_view, int): there is no next valid token:\n"}
                + integer::starts_with(trimmed).to_string());
    }
}    // namespace lex
}    // namespace elder

#endif    // ELDER_LEXER_HPP
