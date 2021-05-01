#include "lexer.hpp"
#include "token.hpp"

#include <ctre.hpp>

#include <algorithm>
#include <optional>
#include <tuple>
#include <variant>
#include <algorithm>

namespace elderLISP {
using namespace token;

namespace {
    using namespace ctre::literals;

    using whitespace = decltype("\\s*"_ctre);

    using lParen = decltype("[(]"_ctre);
    using rParen = decltype("[)]"_ctre);

    using stringLiteral  = decltype("\"([^\"]*)\""_ctre);
    using integerLiteral = decltype("\\d+"_ctre);

    using name   = decltype("(\\w+)"_ctre);
    using equals = decltype("eq"_ctre);

    using truePattern  = decltype("true"_ctre);
    using falsePattern = decltype("false"_ctre);
    using atomic       = decltype("atom"_ctre);

    using first   = decltype("first"_ctre);
    using rest    = decltype("rest"_ctre);
    using combine = decltype("comb"_ctre);

    using condition = decltype("cond"_ctre);
    using let       = decltype("let"_ctre);
    using lambda    = decltype("lambda"_ctre);
    using quote     = decltype("\'"_ctre);
}    // namespace

template<class PatternT, class TokenT>
struct Pattern {
    using token   = TokenT;
    using pattern = PatternT;
};

template<class... Patterns>
auto constexpr make_pattern_array(Patterns... patterns)
{
    using patternVariant = std::variant<Patterns...>;
    return std::array<patternVariant, sizeof...(patterns)>{patterns...};
}

auto constexpr patterns = make_pattern_array(
        Pattern<lParen, LParen>{},
        Pattern<rParen, RParen>{},
        Pattern<stringLiteral, StringLiteral>{},
        Pattern<integerLiteral, IntegerLiteral>{},
        Pattern<truePattern, True>{},
        Pattern<falsePattern, False>{},
        Pattern<atomic, Atomic>{},
        Pattern<equals, Equal>{},
        Pattern<first, First>{},
        Pattern<rest, Rest>{},
        Pattern<combine, Combine>{},
        Pattern<condition, Condition>{},
        Pattern<let, Let>{},
        Pattern<lambda, Lambda>{},
        Pattern<quote, Quote>{},
        Pattern<name, Name>{});

using matchReturnT =
        std::tuple<std::string::const_iterator, std::optional<Token>>;

template<class PatternT, class TokenT>
auto
match(Pattern<PatternT, TokenT>,
      std::string::const_iterator begin,
      std::string::const_iterator end) -> matchReturnT
{
    if(auto const match = PatternT::starts_with(begin, end)) {
        return {begin + match.size(), TokenT{}};
    }

    return {begin, {}};
}

auto
match(Pattern<stringLiteral, StringLiteral>,
      std::string::const_iterator begin,
      std::string::const_iterator end) -> matchReturnT
{
    if(auto const match = stringLiteral::starts_with(begin, end)) {
        return {begin + match.size(),
                StringLiteral{match.get<1>().to_string()}};
    }

    return {begin, {}};
}

auto
match(Pattern<integerLiteral, IntegerLiteral>,
      std::string::const_iterator begin,
      std::string::const_iterator end) -> matchReturnT
{
    if(auto const match = integerLiteral::starts_with(begin, end)) {
        return {begin + match.size(),
                IntegerLiteral{std::stoi(match.to_string())}};
    }

    return {begin, {}};
}

auto
match(Pattern<name, Name>,
      std::string::const_iterator begin,
      std::string::const_iterator end) -> matchReturnT
{
    if(auto const match = name::starts_with(begin, end)) {
        return {begin + match.size(), Name{match.to_string()}};
    }

    return {begin, {}};
}

auto
tokenize(
        std::string::const_iterator begin,
        std::string::const_iterator end,
        std::deque<Token>&& tokens) -> std::deque<Token>
{
    std::vector<matchReturnT> matches{};

    auto const ws = whitespace::starts_with(begin, end);

    for(auto&& pattern : patterns) {
        auto&& result = std::visit(
                [&](auto&& pattern) {
                    return match(pattern, begin + ws.size(), end);
                },
                pattern);

        auto&& [newPos, token] = result;

        if(newPos != begin) {
            matches.push_back(std::move(result));
        }
    }

    if(!matches.empty()) {
        auto&& bestMatch = std::max_element(
                matches.cbegin(),
                matches.cend(),
                [](auto&& lhs, auto&& rhs) {
                    return std::get<0>(lhs) < std::get<0>(rhs);
                });

        auto&& [newPos, token] = *bestMatch;
        if(token.has_value()) {
            tokens.push_back(std::move(token.value()));
        }

        return tokenize(newPos, end, std::move(tokens));
    }

    return std::move(tokens);
}

}    // namespace elderLISP
