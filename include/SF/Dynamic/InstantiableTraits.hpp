#ifndef SF_DYNAMIC_INSTANTIABLE_TRAITS_HPP
#define SF_DYNAMIC_INSTANTIABLE_TRAITS_HPP

#include <SF/Core/Hash.hpp>
// TODO: fix here
#define EXPORT_INSTANTIABLE_KEY(instantiable_key, ...)                                                  \
    template <> struct xxsf_instantiable_traits<__VA_ARGS__> {                                          \
        static ::xxsf_instantiable_traits_key_type key;                                                 \
        static bool xxsf;                                                                               \
    };                                                                                                  \
    ::xxsf_instantiable_traits_key_type xxsf_instantiable_traits<__VA_ARGS__>::key = instantiable_key;  \
    bool xxsf_instantiable_traits<__VA_ARGS__>::xxsf = ::sf::dynamic::instantiable_fixture_t<__VA_ARGS__>::call();

#define EXPORT_INSTANTIABLE(...) EXPORT_INSTANTIABLE_KEY(SF_TYPE_HASH(__VA_ARGS__), __VA_ARGS__)

using xxsf_instantiable_traits_key_type = SF_STATIC_HASH_KEY_TYPE;
static constexpr auto xxsf_instantiable_traits_base_key = xxsf_instantiable_traits_key_type(-1);

template <class InstantiableType>
struct xxsf_instantiable_traits
{
    static constexpr auto key = xxsf_instantiable_traits_base_key;
    static constexpr auto xxsf = false;
};

#endif // SF_DYNAMIC_INSTANTIABLE_TRAITS_HPP
