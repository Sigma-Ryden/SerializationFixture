#ifndef SF_CORE_INSTANTIABLE_TRAITS_HPP
#define SF_CORE_INSTANTIABLE_TRAITS_HPP

#include <SF/Core/Hash.hpp>

#define EXPORT_INSTANTIABLE_KEY(name, ...)                                                              \
    template <>                                                                                         \
    struct xxsf_instantiable_traits<__VA_ARGS__> { static constexpr auto key = SF_STATIC_HASH(name); };

#define EXPORT_INSTANTIABLE(...)                                                                        \
    EXPORT_INSTANTIABLE_KEY(#__VA_ARGS__, __VA_ARGS__)

template <class T>
struct xxsf_instantiable_traits;

template <>
struct xxsf_instantiable_traits<void>
{
    using key_type = SF_STATIC_HASH_KEY_TYPE;
    static constexpr auto base_key = key_type(-1);
};

template <class T>
struct xxsf_instantiable_traits
{
    static constexpr auto key = xxsf_instantiable_traits<void>::base_key;
};

#endif // SF_CORE_INSTANTIABLE_TRAITS_HPP
