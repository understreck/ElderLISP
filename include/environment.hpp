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

template<key_value_pair... KVPs>
struct Environment<KVPs...> {
    std::tuple<KVPs...> kvps;
};

template<key_value_pair... KVPs>
Environment(KVPs...) -> Environment<KVPs...>;

template<key_value_pair... KVPs>
Environment(std::tuple<KVPs...>) -> Environment<KVPs...>;

template<class Env, key_value_pair... KVPs>
requires is_specialisation_of<Env, Environment>
struct Environment<Env, KVPs...> {
    std::tuple<KVPs...> kvps;
    Env outerEnvironment;
};

template<class T>
concept environment = is_specialisation_of<T, Environment>;

template<environment Env, key_value_pair... KVPs>
Environment(std::tuple<KVPs...>, Env) -> Environment<Env, KVPs...>;

template<class Env>
concept has_outer_env = requires(Env env)
{
    env.outerEnvironment;
};

template<key_value_pair... KVPs>
auto consteval push_kvps(environment auto env, std::tuple<KVPs...> kvps)
{
    return Environment{std::tuple_cat(kvps, env.kvps)};
}

template<environment Env, key_value_pair... KVPs>
requires has_outer_env<Env>
auto consteval push_kvps(Env env, std::tuple<KVPs...> kvps)
{
    return Environment{std::tuple_cat(kvps, env.kvps), env.outerEnvironment};
}

template<environment Env>
auto consteval push_scope(Env outer)
{
    return Environment{{}, outer};
}

template<environment Env>
auto consteval pop_scope(Env inner)
{
    static_assert(has_outer_env<Env>, "Needs an outerEnvironment");

    return inner.outerEnvironment;
}

template<size_t i, key_value_pair... KVPs>
auto consteval find_impl(std::tuple<KVPs...> kvps, label auto key)
{
    if constexpr(i < sizeof...(KVPs)) {
        auto kvp = std::get<i>(kvps);

        if constexpr(std::is_same_v<decltype(kvp.key), decltype(key)>) {
            return kvp.value;
        }
        else {
            return find_impl<i + 1>(kvps, key);
        }
    }
    else {
        return key;
    }
}

template<key_value_pair... KVPs>
auto consteval find_impl(std::tuple<KVPs...> kvps, label auto key)
{
    return find_impl<0>(kvps, key);
}

template<environment Env>
requires(!has_outer_env<Env>) auto consteval find_impl(Env env, label auto key)
{
    auto value = find_impl(env.kvps, key);
    //static_assert(
            //!std::is_same_v<decltype(value), decltype(key)>,
            //"Key is not found in this environment");

    return value;
}

template<environment Env>
requires has_outer_env<Env>
auto consteval find_impl(Env env, label auto key)
{
    auto value = find_impl(env.kvps, key);

    if constexpr(std::is_same_v<decltype(value), decltype(key)>) {
        return find_impl(env.outerEnvironment, key);
    }
    else {
        return value;
    }
}

auto consteval find(environment auto env, label auto key)
{
    auto value = find_impl(env, key);
    // static_assert(
    //! std::is_same_v<decltype(value), decltype(key)>,
    //"Could not find key in environment");

    return value;
}
#endif    // ELDERLISTP_ENVIRONMENT_HPP
