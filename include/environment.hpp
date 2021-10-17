#ifndef ELDERLISTP_ENVIRONMENT_HPP
#define ELDERLISTP_ENVIRONMENT_HPP

#include <string_view>
#include <tuple>

#include "list.hpp"

template<atom_or_list Value>
struct KeyValuePair {
    Label key;
    Value value;
};

template<atom_or_list T>
KeyValuePair(std::string_view, T) -> KeyValuePair<T>;

template<class... Ts>
concept key_value_pair = (isSpecalisationOf<Ts, KeyValuePair> && ...);

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
requires isSpecalisationOf<Env, Environment>
struct Environment<Env, KVPs...> {
    std::tuple<KVPs...> kvps;
    Env outerEnvironment;
};

template<class T>
concept environment = isSpecalisationOf<T, Environment>;

template<environment Env, key_value_pair... KVPs>
Environment(std::tuple<KVPs...>, Env) -> Environment<Env, KVPs...>;

template<class Env>
concept has_outer_env = requires(Env env)
{
    env.outerEnvironment;
};

template<key_value_pair... KVPs>
auto constexpr push_kvps(environment auto env, std::tuple<KVPs...> kvps)
{
    return Environment{std::tuple_cat(kvps, env.kvps)};
}

template<environment Env, key_value_pair... KVPs>
requires has_outer_env<Env>
auto constexpr push_kvps(Env env, std::tuple<KVPs...> kvps)
{
    return Environment{std::tuple_cat(kvps, env.kvps), env.outerEnvironment};
}

template<environment Env>
auto constexpr push_scope(Env outer)
{
    return Environment{{}, outer};
}

template<environment Env>
auto constexpr pop_scope(Env inner)
{
    static_assert(has_outer_env<Env>, "Needs an outerEnvironment");

    return inner.outerEnvironment;
}
#endif    // ELDERLISTP_ENVIRONMENT_HPP
