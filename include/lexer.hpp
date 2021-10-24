#ifndef ELDER_LISP_LEXER_HPP
#define ELDER_LISP_LEXER_HPP

#include <string_view>
#include <tuple>
#include <utility>

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
            return is_whitespace(nextChar) || nextChar == '('
                   || nextChar == ')';
        }

        return false;
    }
}

template<size_t position = 0>
auto consteval parse(c_line auto line)
{
    auto constexpr i = next_non_whitespace(line, position);

    if constexpr(compare<i>(line, Line<"lambda">)) {
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
}

auto constexpr p = parse(Line<"' '">);
#endif    // ELDER_LISP_LEXER_HPP
