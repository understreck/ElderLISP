#ifndef ELDERLISP_INTERPRETER_MEMORY_HPP
#define ELDERLISP_INTERPRETER_MEMORY_HPP

#include "ast.hpp"

#include <algorithm>
#include <functional>
#include <optional>
#include <tuple>
#include <string>
#include <vector>

namespace elderLISP {
struct Memory {
    static auto constexpr hasher = std::hash<std::string>{};
    using HashResultType         = decltype(hasher(std::string{}));

    std::vector<std::vector<HashResultType>> keysStack;
    std::vector<std::vector<ast::List>> valuesStack;
};

[[nodiscard]] inline auto
push(Memory memory) -> Memory
{
    memory.keysStack.emplace_back();
    memory.valuesStack.emplace_back();

    return memory;
}

[[nodiscard]] inline auto
drop(Memory memory) -> Memory
{
    auto&& [keysStack, valuesStack] = memory;

    keysStack.pop_back();
    valuesStack.pop_back();

    return memory;
}

[[nodiscard]] inline auto
set_element(Memory memory, std::string const& key, ast::List value) -> Memory
{
    auto const hash = Memory::hasher(key);

    auto&& [keysStack, valuesStack] = memory;
    auto&& topKeys                  = *keysStack.rbegin();
    auto&& topValues                = *valuesStack.rbegin();

    topKeys.push_back(hash);
    topValues.push_back(std::move(value));

    return memory;
}

[[nodiscard]] inline auto
element_index(
        std::vector<Memory::HashResultType> const& keys,
        Memory::HashResultType hash) -> std::optional<size_t>
{
    auto&& position = std::find(keys.crbegin(), keys.crend(), hash);

    return position != keys.crend() ? (position.base() - keys.cbegin())
                                    : std::optional<size_t>{};
}

[[nodiscard]] inline auto
element_index(Memory const& memory, Memory::HashResultType hash)
        -> std::optional<std::tuple<size_t, size_t>>
{
    for(auto stackIndex = int(memory.keysStack.size() - 1); stackIndex >= 0;
        stackIndex--) {
        if(auto const hashIndex =
                   element_index(memory.keysStack[stackIndex], hash);
           hashIndex.has_value()) {
            return {{stackIndex, hashIndex.value()}};
        }
    }

    return {};
}

[[nodiscard]] inline auto
get_element(Memory const& memory, std::string const& key) -> ast::List
{
    auto const hash = Memory::hasher(key);

    auto const [stackIndex, valueIndex] = element_index(memory, hash).value();

    return memory.valuesStack[stackIndex][valueIndex];
}

}    // namespace elderLISP

#endif    // ELDERLISP_INTERPRETER_MEMORY_HPP
