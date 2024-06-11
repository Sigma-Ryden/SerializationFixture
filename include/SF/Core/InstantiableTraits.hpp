#ifndef SF_CORE_INSTANTIABLE_TRAITS_HPP
#define SF_CORE_INSTANTIABLE_TRAITS_HPP

#include <SF/Core/Hash.hpp>

#define EXPORT_INSTANTIABLE_KEY(unique_name, ...)                                                       \
    namespace sf { namespace core {                                                                     \
        template <> struct instantiable_traits_key_t<__VA_ARGS__> {                                     \
            static constexpr auto key = SF_STATIC_HASH(unique_name);                                    \
        };                                                                                              \
    }}

#define EXPORT_INSTANTIABLE(...)                                                                        \
    EXPORT_INSTANTIABLE_KEY(#__VA_ARGS__, __VA_ARGS__)

namespace sf
{

namespace core
{

struct instantiable_traits_t
{
    using key_type = SF_STATIC_HASH_KEY_TYPE;
    
    static constexpr auto base_key = key_type(-1);
};

template <class T>
struct instantiable_traits_key_t
{
    static constexpr auto key = instantiable_traits_t::base_key;
};

} // namespace core

} // namespace sf

#endif // SF_CORE_INSTANTIABLE_TRAITS_HPP
