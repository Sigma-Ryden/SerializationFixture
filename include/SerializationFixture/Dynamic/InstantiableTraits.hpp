#ifndef SF_DYNAMIC_INSTANTIABLE_TRAITS_HPP
#define SF_DYNAMIC_INSTANTIABLE_TRAITS_HPP

#include <SerializationFixture/Core/Hash.hpp>

#define EXPORT_INSTANTIABLE(...)                                                                        \
    template <> struct xxsf_instantiable_fixture<__VA_ARGS__> { static bool fixture; };                 \
    bool xxsf_instantiable_fixture<__VA_ARGS__>::fixture =                                              \
        ::sf::dynamic::instantiable_registry_t::instance().fixture<__VA_ARGS__>();

#define EXPORT_INSTANTIABLE_KEY(instantiable_key, ...)                                                  \
    template <> struct xxsf_instantiable_traits<__VA_ARGS__> {                                          \
        static ::xxsf_instantiable_traits_key_type key() { return instantiable_key; }                   \
    };                                                                                                  \
    EXPORT_INSTANTIABLE(__VA_ARGS__)

using xxsf_instantiable_traits_key_type = ::sf::let::u64;
static constexpr auto xxsf_instantiable_traits_base_key = xxsf_instantiable_traits_key_type(-1);

template <class InstantiableType>
struct xxsf_instantiable_traits
{
    static ::xxsf_instantiable_traits_key_type key() { return SF_TYPE_HASH(InstantiableType); }
};

template <class InstantiableType>
struct xxsf_instantiable_fixture;

#endif // SF_DYNAMIC_INSTANTIABLE_TRAITS_HPP
