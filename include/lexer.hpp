#ifndef ELDERLISP_LEXER_HPP
#define ELDERLISP_LEXER_HPP

#include <iterator>
#include <sstream>
#include <memory>
#include <string_view>
#include <variant>
#include <functional>
#include <iostream>

struct Atom {
    std::string const name;

    auto inline print() const -> void
    {
        std::cout << "Atom: " << name << " ";
    }
};

struct NIL {
    auto static print() -> void
    {
        std::cout << "NIL ";
    }
};

struct LPeren {
    int const depth;

    auto inline print() const -> void
    {
        std::cout << "LPeren: " << depth << " ";
    }
};

struct RPeren {
    int const depth;

    auto inline print() const -> void
    {
        std::cout << "RPeren: " << depth << " ";
    }
};

using Token = std::variant<NIL, Atom, LPeren, RPeren>;

auto inline nextToken(
        std::tuple<std::string_view, int> const input,
        int const depth) -> std::tuple<size_t, Token>
{
    auto&& [string, position] = input;

    auto&& nextSpace = std::find_if(
            std::begin(string) + position,
            std::end(string),
            [](auto c) { return c == ' '; });

    if(nextSpace == std::end(string)) {
        return {std::end(string) - std::begin(string), NIL{}};
    }
}

#endif    // ELDERLISP_LEXER_HPP
