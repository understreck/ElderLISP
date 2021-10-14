#ifndef ELDERLISTP_ENVIRONMENT_HPP
#define ELDERLISTP_ENVIRONMENT_HPP

#include "list.hpp"

template<atom_or_list Key, atom_or_list Value>
using KeyValuePair = List<Key, Value>;

template<class... Ts>
concept key_value_pair = (isSpecalisationOf<Ts, KeyValuePair> && ...);

template<class...>
class Environment {};

template<key_value_pair... KVPs>
struct Environment<KVPs...> {
    List<KVPs...> kvps;
};

template<class Env, key_value_pair... KVPs>
requires isSpecalisationOf<Env, Environment>
struct Environment<Env, KVPs...> {
    List<KVPs...> kvps;
    Env outerEnvironment;
};

#endif    // ELDERLISTP_ENVIRONMENT_HPP
