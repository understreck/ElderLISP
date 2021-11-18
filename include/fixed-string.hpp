#ifndef ELDERLISP_FIXED_STRING_HPP
#define ELDERLISP_FIXED_STRING_HPP

#include <cstddef>
#include <array>
#include <type_traits>
#include <string_view>

template<size_t N>
struct FixedString : std::array<char, N> {
    template<size_t... indices>
    consteval FixedString(
            char const (&str)[N + 1],
            std::index_sequence<indices...>) :
                std::array<char, N>{str[indices]...}
    {}

    consteval FixedString(char const (&str)[N + 1]) :
                FixedString{str, std::make_index_sequence<N>{}}
    {}

    template<class... C>
    constexpr FixedString(C... cs) : std::array<char, N>{cs...}
    {}

    constexpr operator std::string_view() const
    {
        return std::string_view{this->data(), N};
    }
};

template<size_t N>
FixedString(char const (&)[N]) -> FixedString<N - 1>;

template<
        size_t begin,
        size_t end,
        FixedString line,
        template<auto>
        class Wrapper>
auto consteval substring(Wrapper<line>)
{
    return []<size_t... indices>(std::index_sequence<indices...>)
    {
        return FixedString<end - begin>{line[indices + begin]...};
    }
    (std::make_index_sequence<end - begin>{});
}

#endif    // ELDERLISP_FIXED_STRING_HPP
