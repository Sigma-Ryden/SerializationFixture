#ifndef SF_CORE_TYPE_REGISTRY_HPP
#define SF_CORE_TYPE_REGISTRY_HPP

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

#define CONDITIONAL_TYPE_REGISTRY(...)                                                                  \
    namespace sf { namespace core {                                                                     \
        template <typename T> struct TypeRegistry<T, SFWHEN(__VA_ARGS__)> : std::true_type {};          \
    }}

// you should use TYPE_REGISTRY before using EXTERN_SERIALIZATION e.t.
#define TYPE_REGISTRY(...)                                                                              \
    namespace sf { namespace core {                                                                     \
        template <> struct TypeRegistry<__VA_ARGS__> : std::true_type {};                               \
    }}

namespace sf
{

namespace core
{

template <typename T, typename enable = void>
struct TypeRegistry : std::false_type {};

} // namespace core

namespace meta
{

template <typename T> struct is_registered : boolean<core::TypeRegistry<T>::value> {};

// use this function only for extern type registry check
template <typename T> struct is_registered_extern
#ifdef SF_TYPE_REGISTRY_DISABLE
    : std::true_type {};
#else
    : is_registered<T> {};
#endif // SF_TYPE_REGISTRY_DISABLE

} // namespace meta

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_unsupported<T>::value)

#endif // SF_CORE_TYPE_REGISTRY_HPP
