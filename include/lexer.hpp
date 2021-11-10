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

template<size_t position, FixedString line>
auto consteval next_end_of_token(LineT<line>)
{
    for(auto i = position; i < line.size(); i++) {
        if(is_end_of_token(line[i])) {
            return i;
        }
    }

    return line.size();
}

template<class... elements, size_t... indices>
auto consteval make_list_from(
        std::tuple<elements...> es,
        std::index_sequence<indices...>)
{
    return ListT{std::get<indices>(es)..., NIL};
}

template<class... elements>
auto consteval make_list_from(std::tuple<elements...> es)
{
    return make_list_from(es, std::make_index_sequence<sizeof...(elements)>{});
}

auto consteval make_list_from(std::tuple<>)
{
    return NIL;
}

template<size_t position = 0, class... Els>
auto consteval parse_list(c_line auto line, Els... els)
{
    auto constexpr i = next_non_whitespace(line, position);

    if constexpr(i >= line.value.size()) {
        static_assert(
                std::is_same_v<decltype(line), void>,
                "Reached EOL prematurely");
        return;
    }
    else if constexpr(line.value[i] == '(') {
        auto constexpr eolResult = parse_list<i + 1>(line);
        auto constexpr l         = make_list_from(eolResult.elements);

        return parse_list<eolResult.nextPos>(line, els..., l);
    }
    else if constexpr(line.value[i] == ')') {
        struct EndOfList {
            std::tuple<Els...> elements;
            size_t nextPos;
        };
        return EndOfList{{els...}, i + 1};
    }
    else if constexpr(compare<i>(line, Line<"lambda">)) {
        return parse_list<i + 6>(line, els..., CI<LAMBDA>);
    }
    else if constexpr(compare<i>(line, Line<"define">)) {
        return parse_list<i + 6>(line, els..., CI<DEFINE>);
    }
    else if constexpr(compare<i>(line, Line<"quote">)) {
        return parse_list<i + 5>(line, els..., CI<QUOTE>);
    }
    else if constexpr(compare<i>(line, Line<"atom?">)) {
        return parse_list<i + 5>(line, els..., CI<ATOM>);
    }
    else if constexpr(compare<i>(line, Line<"eq?">)) {
        return parse_list<i + 3>(line, els..., CI<EQUAL>);
    }
    else if constexpr(compare<i>(line, Line<"car">)) {
        return parse_list<i + 3>(line, els..., CI<FIRST>);
    }
    else if constexpr(compare<i>(line, Line<"cdr">)) {
        return parse_list<i + 3>(line, els..., CI<REST>);
    }
    else if constexpr(compare<i>(line, Line<"cons">)) {
        return parse_list<i + 4>(line, els..., CI<COMBINE>);
    }
    else if constexpr(compare<i>(line, Line<"if">)) {
        return parse_list<i + 2>(line, els..., CI<IF>);
    }
    else if constexpr(compare<i>(line, Line<"list">)) {
        return parse_list<i + 4>(line, els..., CI<LIST>);
    }
    else if constexpr(compare<i>(line, Line<"*">)) {
        return parse_list<i + 1>(line, els..., CI<MUL>);
    }
    else if constexpr(compare<i>(line, Line<"-">)) {
        return parse_list<i + 1>(line, els..., CI<SUB>);
    }
    else if constexpr(compare<i>(line, Line<"+">)) {
        return parse_list<i + 1>(line, els..., CI<ADD>);
    }
    else if constexpr(compare<i>(line, Line<"/">)) {
        return parse_list<i + 1>(line, els..., CI<DIV>);
    }
    else if constexpr(compare<i>(line, Line<"%">)) {
        return parse_list<i + 1>(line, els..., CI<MOD>);
    }
    else if constexpr(compare<i>(line, Line<"F">)) {
        return parse_list<i + 1>(line, els..., False);
    }
    else if constexpr(compare<i>(line, Line<"T">)) {
        return parse_list<i + 1>(line, els..., True);
    }
    else if constexpr(line.value[i] == '\'') {
        static_assert(
                i + 2 < line.value.size(),
                "Trying to construct a character past the end of the string");
        static_assert(
                line.value[i + 2] == '\'',
                "No closing ' character when constructing Char");

        return parse_list<i + 3>(line, els..., C<line.value[i + 1]>);
    }
    else if constexpr(auto constexpr number = parse_integer<i>(line)) {
        auto constexpr result = *number;
        return parse_list<i + result.length + result.sign>(
                line,
                els...,
                Int<result.value>);
    }
    else {
        auto constexpr endOfToken = next_end_of_token<i>(line);
        return parse_list<endOfToken>(
                line,
                els...,
                Lbl<substring<i, endOfToken>(line)>);
    }
}

template<size_t position = 0>
auto consteval parse(c_line auto line)
{
    auto constexpr i = next_non_whitespace(line, position);

    if constexpr(i >= line.value.size()) {
        static_assert(
                std::is_same_v<decltype(line), void>,
                "Reached EOL prematurely");
        return;
    }
    else if constexpr(line.value[i] == '(') {
        return make_list_from(parse_list<i + 1>(line).elements);
    }
    else if constexpr(line.value[i] == ')') {
        static_assert(
                std::is_same_v<decltype(line), void>,
                "Trying to close a list without one open");
        return;
    }
    else if constexpr(compare<i>(line, Line<"lambda">)) {
        return CI<LAMBDA>;
    }
    else if constexpr(compare<i>(line, Line<"define">)) {
        return CI<DEFINE>;
    }
    else if constexpr(compare<i>(line, Line<"quote">)) {
        return CI<QUOTE>;
    }
    else if constexpr(compare<i>(line, Line<"atom?">)) {
        return CI<ATOM>;
    }
    else if constexpr(compare<i>(line, Line<"eq?">)) {
        return CI<EQUAL>;
    }
    else if constexpr(compare<i>(line, Line<"car">)) {
        return CI<FIRST>;
    }
    else if constexpr(compare<i>(line, Line<"cdr">)) {
        return CI<REST>;
    }
    else if constexpr(compare<i>(line, Line<"cons">)) {
        return CI<COMBINE>;
    }
    else if constexpr(compare<i>(line, Line<"if">)) {
        return CI<IF>;
    }
    else if constexpr(compare<i>(line, Line<"list">)) {
        return CI<LIST>;
    }
    else if constexpr(compare<i>(line, Line<"*">)) {
        return CI<MUL>;
    }
    else if constexpr(compare<i>(line, Line<"-">)) {
        return CI<SUB>;
    }
    else if constexpr(compare<i>(line, Line<"+">)) {
        return CI<ADD>;
    }
    else if constexpr(compare<i>(line, Line<"/">)) {
        return CI<DIV>;
    }
    else if constexpr(compare<i>(line, Line<"%">)) {
        return CI<MOD>;
    }
    else if constexpr(compare<i>(line, Line<"F">)) {
        return False;
    }
    else if constexpr(compare<i>(line, Line<"T">)) {
        return True;
    }
    else if constexpr(line.value[i] == '\'') {
        static_assert(
                i + 2 < line.value.size(),
                "Trying to construct a character past the end of the string");
        static_assert(
                line.value[i + 2] == '\'',
                "No closing ' character when constructing Char");

        return C<line.value[i + 1]>;
    }
    else if constexpr(auto constexpr number = parse_integer<i>(line)) {
        auto constexpr result = *number;
        return Int<result.value>;
    }
    else {
        auto constexpr endOfToken = next_end_of_token<i>(line);
        return Lbl<substring<i, endOfToken>(line)>;
    }
}
#endif    // ELDER_LISP_LEXER_HPP
