#ifndef ELDERLISTP_LIST_HPP
#define ELDERLISTP_LIST_HPP

#include <cstddef>
#include <string_view>
#include <type_traits>

#include "fixed-string.hpp"
#include "atomic-types.hpp"

template<class...>
struct ListT;

template<>
struct ListT<> {};

auto constexpr NIL = ListT<>{};

template<class T>
concept nil = std::is_same_v<T, ListT<>>;

template<class...>
struct Procedure;

template<class T>
concept procedure = is_specialisation_of<T, Procedure>;

template<class T>
concept atom = nil<T> || data_type<T> || procedure<T>;

template<class T>
auto consteval is_atom(T)
{
    return Bool<atom<T>>;
}

template<class T>
concept atom_not_nil = atom<T> && !nil<T>;

template<class T>
concept list = is_specialisation_of<T, ListT>;

template<class T>
concept list_not_nil = list<T> && !nil<T>;

template<class... Ts>
concept atom_or_list = ((atom<Ts> || list<Ts>)&&...);

template<class... Ts>
concept atom_or_list_not_nil = ((atom_not_nil<Ts> || list_not_nil<Ts>)&&...);

template<class T, class U>
struct ListT<T, U> {
    T car;
    U cdr;

    constexpr ListT(T t, U u) : car{t}, cdr{u}
    {}

    constexpr ListT() = default;
};

template<class T, class U, class... Us>
struct ListT<T, U, Us...> {
    T car;
    ListT<U, Us...> cdr;

    constexpr ListT(T t, U u, Us... us) :
                car{t}, cdr{u, us...}
    {}

    constexpr ListT() = default;
};

template<class... Ts>
ListT(Ts...) -> ListT<Ts...>;

auto consteval List()
{
    return NIL;
}

template<atom_or_list_not_nil Element>
auto consteval List(Element element)
{
    return ListT{element, NIL};
}

auto consteval List(atom_or_list auto element, atom_or_list auto... elements)
{
    return ListT{element, elements..., NIL};
}

#endif    // ELDERLISTP_LIST_HPP
