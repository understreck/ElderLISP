#include "lexer.hpp"

#include <algorithm>
#include <ctre.hpp>
#include <tuple>

namespace lexer {

namespace patterns {
using namespace ctre::literals;

using lPeren = decltype("\\s*[(]"_ctre);
using rPeren = decltype("\\s*[)]"_ctre);

using atom   = decltype("\\s*(\\w+)"_ctre);
using equals = decltype("\\s*[eq]"_ctre);

using first   = decltype("\\s*first"_ctre);
using rest    = decltype("\\s*rest"_ctre);
using combine = decltype("\\s*comb"_ctre);

using condition = decltype("\\s*if"_ctre);
using let       = decltype("\\s*let"_ctre);
using quote     = decltype("\\s*\""_ctre);
}    // namespace patterns

auto
tokenize(
        std::string::const_iterator begin,
        std::string::const_iterator end,
        std::deque<Token> tokens) -> std::deque<Token>
{
    if(auto match = patterns::lPeren::starts_with(begin, end)) {
        tokens.push_back(LPeren{});
        return tokenize(begin + match.size(), end, std::move(tokens));
    }

    if(auto match = patterns::rPeren::starts_with(begin, end)) {
        tokens.push_back(RPeren{});
        return tokenize(begin + match.size(), end, std::move(tokens));
    }

    if(auto match = patterns::atom::starts_with(begin, end)) {
        tokens.emplace_back(Atom{match.to_string()});
        return tokenize(begin + match.size(), end, std::move(tokens));
    }

    if(auto match = patterns::equals::starts_with(begin, end)) {
        tokens.push_back(Equals{});
        return tokenize(begin + match.size(), end, std::move(tokens));
    }

    if(auto match = patterns::first::starts_with(begin, end)) {
        tokens.push_back(First{});
        return tokenize(begin + match.size(), end, std::move(tokens));
    }

    if(auto match = patterns::rest::starts_with(begin, end)) {
        tokens.push_back(Rest{});
        return tokenize(begin + match.size(), end, std::move(tokens));
    }

    if(auto match = patterns::combine::starts_with(begin, end)) {
        tokens.push_back(Combine{});
        return tokenize(begin + match.size(), end, std::move(tokens));
    }

    if(auto match = patterns::condition::starts_with(begin, end)) {
        tokens.push_back(Condition{});
        return tokenize(begin + match.size(), end, std::move(tokens));
    }

    if(auto match = patterns::let::starts_with(begin, end)) {
        tokens.push_back(Let{});
        return tokenize(begin + match.size(), end, std::move(tokens));
    }

    if(auto match = patterns::quote::starts_with(begin, end)) {
        tokens.push_back(Quote{});
        return tokenize(begin + match.size(), end, std::move(tokens));
    }

    return tokens;
}

}
