#ifndef SF_CORE_INSTANTIABLE_TRAITS_HPP
#define SF_CORE_INSTANTIABLE_TRAITS_HPP

#include <SF/Core/Hash.hpp>

#define EXPORT_INSTANTIABLE_KEY(name, ...)                                                              \
    template <> struct xxsf_instantiable_traits<__VA_ARGS__> {                                          \
        static constexpr auto key = SF_STATIC_HASH(name);                                               \
    };

#define EXPORT_INSTANTIABLE(...)                                                                        \
    EXPORT_INSTANTIABLE_KEY(#__VA_ARGS__, __VA_ARGS__)

using xxsf_instantiable_traits_key_type = SF_STATIC_HASH_KEY_TYPE;
static constexpr auto xxsf_instantiable_traits_base_key = xxsf_instantiable_traits_key_type(-1);

template <class InstantiableType>
struct xxsf_instantiable_traits
{
    static constexpr auto key = xxsf_instantiable_traits_base_key;
};

#endif // SF_CORE_INSTANTIABLE_TRAITS_HPP
