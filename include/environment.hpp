#ifndef ELDERLISTP_ENVIRONMENT_HPP
#define ELDERLISTP_ENVIRONMENT_HPP

#include <string_view>
#include <tuple>

#include "list.hpp"

template<label Label, atom_or_list Value>
struct KeyValuePair {
    Label key;
    Value value;
};

template<label Label, atom_or_list Value>
KeyValuePair(Label, Value) -> KeyValuePair<Label, Value>;

template<class... Ts>
concept key_value_pair = (is_specialisation_of<Ts, KeyValuePair> && ...);

template<class...>
struct Environment;

template<class T>
concept environment = is_specialisation_of<T, Environment>;

template<key_value_pair... KVPs>
struct Environment<KVPs...> {
    std::tuple<KVPs...> kvps;
};

template<key_value_pair... KVPs>
Environment(KVPs...) -> Environment<KVPs...>;

template<key_value_pair... KVPs>
Environment(std::tuple<KVPs...>) -> Environment<KVPs...>;

template<environment Env, key_value_pair... KVPs>
struct Environment<Env, KVPs...> {
    Env parent;
    std::tuple<KVPs...> kvps;
};

template<environment Env, key_value_pair... KVPs>
Environment(Env, std::tuple<KVPs...>) -> Environment<Env, KVPs...>;

template<class Env>
concept has_parent = requires(Env env)
{
    env.parent;
};

template<environment Env>
requires(!has_parent<Env>) auto consteval push_kvp(
        Env env,
        key_value_pair auto kvp)
{
    return Environment{std::tuple_cat(std::tuple{kvp}, env.kvps)};
}

template<environment Env>
requires has_parent<Env>
auto consteval push_kvp(Env env, key_value_pair auto kvp)
{
    return Environment{env, std::tuple_cat(std::tuple{kvp}, env.kvps)};
}

template<environment Env>
auto consteval push_scope(Env outer)
{
    return Environment{outer, {}};
}

template<environment Env>
auto consteval pop_scope(Env inner)
{
    static_assert(has_parent<Env>, "Needs a parent Environment");

    return inner.parent;
}

template<size_t index = 0, key_value_pair... KVPs>
auto consteval find(std::tuple<KVPs...> kvps, label auto key)
{
    if constexpr(index < sizeof...(KVPs)) {
        if constexpr(equal(std::get<index>(kvps).key, key)) {
            return std::get<index>(kvps).value;
        }
        else {
            return find<index + 1>(kvps, key);
        }
    }
    else {
        return key;
    }
}

template<environment Env>
auto consteval find(Env env, label auto key)
{
    auto value = find(env.kvps, key);

    if constexpr(equal(value, key)) {
        if constexpr(has_parent<Env>) {
            return find(env.parent, key);
        }
        else {
            return key;
        }
    }
    else {
        return value;
    }
}
#endif    // ELDERLISTP_ENVIRONMENT_HPP
