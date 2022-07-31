#ifndef ELDER_LEXER_HPP
#define ELDER_LEXER_HPP

#include "interpreter.hpp"

#include <ctre.hpp>

#include <string>
#include <string_view>
#include <span>
#include <ranges>
#include <tuple>

namespace elder {
namespace lex {
    inline auto constexpr matches = std::tuple{
            ctre::match<"\\s*">,         // whitespace
            ctre::match<"[(]">,          // left peren
            ctre::match<"[)]">,          // right peren
            ctre::match<"quote|\'">,     // quote
            ctre::match<"list">,         // list
            ctre::match<"front|car">,    // front|car
            ctre::match<"rest|cdr">,     // rest|cdr
            ctre::match<"join|cons">,    // join|cons
            ctre::match<"atom">,         // atom
            ctre::match<"=">,            // equal
            ctre::match<"def">,          // define
            ctre::match<"y|lambda">,     // lambda
            ctre::match<"\\?">,          // conditional
            ctre::match<"(\\d+)">,       // integer
            ctre::match<"(\".*\")">,     // string
            ctre::match<"([^\\s]+)">,    // symbol
    };

}
}    // namespace elder

#endif    // ELDER_LEXER_HPP
