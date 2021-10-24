#ifndef ELDER_LISP_LEXER_HPP
#define ELDER_LISP_LEXER_HPP

#include <string_view>
#include <tuple>
#include <utility>
#include <optional>
#include <math.h>

#include "list.hpp"

template<FixedString string>
struct LineT : std::integral_constant<decltype(string), string> {};

template<FixedString string>
auto constexpr Line = LineT<string>{};

// auto constexpr l = FixedString{"lambda"};

template<class T>
struct IsLineT : std::false_type {};

template<FixedString s>
struct IsLineT<LineT<s>> : std::true_type {};

template<class T>
concept c_line = IsLineT<T>::value;

// template<size_t position, class... Elements>
// auto consteval parse(c_line auto line, std::tuple<Elements...> elements)
//{}

auto consteval is_whitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

template<FixedString line>
auto consteval next_non_whitespace(LineT<line>, size_t position)
{
    for(auto i = position; i < line.size(); i++) {
        if(!is_whitespace(line[i])) {
            return i;
        }
    }

    return line.size();
}

auto consteval is_end_of_token(char nextChar)
{
    return is_whitespace(nextChar) || nextChar == '(' || nextChar == ')';
}

template<size_t start, FixedString line, FixedString token>
auto consteval compare(LineT<line>, LineT<token>)
{
    if constexpr(
            auto constexpr beyondEnd = (start + token.size()) > line.size()) {
        return false;
    }
    else {
        auto constexpr begin = line.begin() + start;
        auto constexpr end   = begin + token.size();

        if(std::string_view{begin, end} == token) {
            if(end == line.end()) {
                return true;
            }

            auto nextChar = *end;
            return is_end_of_token(nextChar);
        }

        return false;
    }
}

auto consteval is_digit(char c)
{
    return ('0' <= c) && (c <= '9');
}

struct ParseIntegerResult {
    long value;
    int length;
    bool sign;
};
// returns 0 if it is not a valid number
template<size_t start, FixedString line>
auto consteval parse_integer(LineT<line>) -> std::optional<ParseIntegerResult>
{
    long sign = 1;
    if(!is_digit(line[start])) {
        if(line[start] != '-') {
            return std::nullopt;
        }

        sign = -1;
    }
    int length  = 0;
    long number = 0;

    for(auto i = sign == 1 ? start : start + 1; i < line.size(); i++) {
        if(!is_digit(line[i])) {
            if(is_end_of_token(line[i])) {
                return {
                        {.value  = number * sign,
                         .length = length,
                         .sign   = sign == -1}};
            }

            return std::nullopt;
        }

        length += 1;
        number = number * 10 + (line[i] - '0');
    }

    return {{.value = number * sign, .length = length, .sign = sign == -1}};
}

template<class T>
struct ParseResult {
    int nextPos;
    T value;
};

template<class T>
ParseResult(int, T) -> ParseResult<T>;

template<size_t position = 0>
auto consteval parse(c_line auto line)
{
    auto constexpr i = next_non_whitespace(line, position);

    static_assert(i != line.value.size(), "Reached EOL prematurely");

    if constexpr(compare<i>(line, Line<"lambda">)) {
        return ParseResult{i + 6, CI<LAMBDA>};
    }
    else if constexpr(compare<i>(line, Line<"define">)) {
        return ParseResult{i + 6, CI<DEFINE>};
    }
    else if constexpr(compare<i>(line, Line<"quote">)) {
        return ParseResult{i + 5, CI<QUOTE>};
    }
    else if constexpr(compare<i>(line, Line<"atom?">)) {
        return ParseResult{i + 5, CI<ATOM>};
    }
    else if constexpr(compare<i>(line, Line<"eq?">)) {
        return ParseResult{i + 3, CI<EQUAL>};
    }
    else if constexpr(compare<i>(line, Line<"car">)) {
        return ParseResult{i + 3, CI<FIRST>};
    }
    else if constexpr(compare<i>(line, Line<"cdr">)) {
        return ParseResult{i + 3, CI<REST>};
    }
    else if constexpr(compare<i>(line, Line<"cons">)) {
        return ParseResult{i + 4, CI<COMBINE>};
    }
    else if constexpr(compare<i>(line, Line<"if">)) {
        return ParseResult{i + 2, CI<IF>};
    }
    else if constexpr(compare<i>(line, Line<"list">)) {
        return ParseResult{i + 4, CI<LIST>};
    }
    else if constexpr(compare<i>(line, Line<"*">)) {
        return ParseResult{i + 1, CI<MUL>};
    }
    else if constexpr(compare<i>(line, Line<"-">)) {
        return ParseResult{i + 1, CI<SUB>};
    }
    else if constexpr(compare<i>(line, Line<"+">)) {
        return ParseResult{i + 1, CI<ADD>};
    }
    else if constexpr(compare<i>(line, Line<"/">)) {
        return ParseResult{i + 1, CI<DIV>};
    }
    else if constexpr(compare<i>(line, Line<"%">)) {
        return ParseResult{i + 1, CI<MOD>};
    }
    else if constexpr(compare<i>(line, Line<"F">)) {
        return ParseResult{i + 1, False};
    }
    else if constexpr(compare<i>(line, Line<"T">)) {
        return ParseResult{i + 1, True};
    }
    else if constexpr(line.value[i] == '\'') {
        static_assert(
                i + 2 < line.value.size(),
                "Trying to construct a character past the end of the string");
        static_assert(
                line.value[i + 2] == '\'',
                "No closing ' character when constructing Char");

        return ParseResult{i + 3, C<line.value[i + 1]>};
    }
    else if constexpr(auto constexpr number = parse_integer<i>(line)) {
        auto constexpr result = *number;
        return ParseResult{i + result.length + result.sign, Int<result.value>};
    }
    // else if constexpr() {
    //}
}

auto constexpr p = parse(Line<" -001">);
#endif    // ELDER_LISP_LEXER_HPP
