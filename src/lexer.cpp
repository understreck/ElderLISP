#include "lexer.hpp"

#include <algorithm>
#include <ctre.hpp>
#include <tuple>

using namespace ctre::literals;

using lPerenPattern = decltype("\\s*[(]"_ctre);
using rPerenPattern = decltype("\\s*[)]"_ctre);

using atomPattern = decltype("\\s*(\\w+)"_ctre);

auto
nextToken(std::string::const_iterator input, std::string::const_iterator end)
        -> std::tuple<Token, std::string::const_iterator>
{
    auto&& [token, it] = std::tuple{Token{NIL{}}, end};

    if(auto match = lPerenPattern::starts_with(input, end)) {
        token = LPeren{};
        it    = input + match.size();
    }
    else if(auto match = rPerenPattern::starts_with(input, end)) {
        token = RPeren{};
        it    = input + match.size();
    }
    else if(auto match = atomPattern::starts_with(input, end)) {
        token = Atom{match.get<1>().to_string()};
        it    = input + match.size();
    }

    return {token, it};
}

auto
tokenize(
        std::string::const_iterator begin,
        std::string::const_iterator end,
        std::deque<Token> tokens) -> std::deque<Token>
{
    if(auto match = lPerenPattern::starts_with(begin, end)) {
        tokens.push_back(LPeren{});
        return tokenize(begin + match.size(), end, std::move(tokens));
    }

    if(auto match = rPerenPattern::starts_with(begin, end)) {
        tokens.push_back(RPeren{});
        return tokenize(begin + match.size(), end, std::move(tokens));
    }

    if(auto match = atomPattern::starts_with(begin, end)) {
        tokens.emplace_back(Atom{match.to_string()});
        return tokenize(begin + match.size(), end, std::move(tokens));
    }

    return tokens;
}
