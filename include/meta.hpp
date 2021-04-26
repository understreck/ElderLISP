namespace elderLISP {
namespace meta {

    template<class T, class... Us>
    struct Index {};

    template<class T, class... Us>
    struct Index<T, T, Us...> {
        static auto constexpr value = 0ul;
    };

    template<class T, class U, class... Vs>
    struct Index<T, U, Vs...> {
        static auto constexpr value = 1ul + Index<T, Vs...>::value;
    };

    template<class T, class... Us>
    auto constexpr index = Index<T, Us...>::value;

    template<class T, class U>
    struct VariantIndex {};

    template<class T, class... Us>
    struct VariantIndex<T, std::variant<Us...>> {
        static auto constexpr value = index<T, Us...>;
    };

    template<class T, class Variant>
    auto constexpr variantIndex = VariantIndex<T, Variant>::value;
}    // namespace meta
}    // namespace elderLISP
