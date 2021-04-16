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

    using atom   = decltype("(\\w+)"_ctre);
    using equals = decltype("eq"_ctre);

    using first   = decltype("first"_ctre);
    using rest    = decltype("rest"_ctre);
    using combine = decltype("comb"_ctre);

    using condition = decltype("if"_ctre);
    using let       = decltype("let"_ctre);
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
        Pattern<whitespace, Whitespace>{},
        Pattern<lParen, LParen>{},
        Pattern<rParen, RParen>{},
        Pattern<stringLiteral, StringLiteral>{},
        Pattern<integerLiteral, IntegerLiteral>{},
        Pattern<equals, Equals>{},
        Pattern<first, First>{},
        Pattern<rest, Rest>{},
        Pattern<combine, Combine>{},
        Pattern<let, Let>{},
        Pattern<quote, Quote>{},
        Pattern<atom, Atom>{});

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
match(Pattern<whitespace, Whitespace>,
      std::string::const_iterator begin,
      std::string::const_iterator end) -> matchReturnT
{
    if(auto const match = whitespace::starts_with(begin, end)) {
        return {begin + match.size(), {}};
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
match(Pattern<atom, Atom>,
      std::string::const_iterator begin,
      std::string::const_iterator end) -> matchReturnT
{
    if(auto const match = atom::starts_with(begin, end)) {
        return {begin + match.size(), Atom{match.to_string()}};
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

    for(auto&& pattern : patterns) {
        auto&& result = std::visit(
                [&](auto&& pattern) { return match(pattern, begin, end); },
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
