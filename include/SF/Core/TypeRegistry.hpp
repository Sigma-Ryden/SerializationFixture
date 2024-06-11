#ifndef SF_CORE_TYPE_REGISTRY_HPP
#define SF_CORE_TYPE_REGISTRY_HPP

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

#define CONDITIONAL_TYPE_REGISTRY(...)                                                                  \
    namespace sf { namespace core {                                                                     \
        template <typename T> struct type_registry_t<T, SF_WHEN(__VA_ARGS__)> : std::true_type {};      \
    }}

// you should use TYPE_REGISTRY before using EXTERN_SERIALIZATION e.t.
#define TYPE_REGISTRY(...)                                                                              \
    namespace sf { namespace core {                                                                     \
        template <> struct type_registry_t<__VA_ARGS__> : std::true_type {};                            \
    }}

namespace sf
{

namespace core
{

template <typename T, typename enable = void>
struct type_registry_t : std::false_type {};

} // namespace core

namespace meta
{

template <typename T> struct is_registered : core::type_registry_t<T> {};

// use this function only for extern type registry check
template <typename T> struct is_registered_extern
#ifdef SF_TYPE_REGISTRY_DISABLE
    : std::true_type {};
#else
    : is_registered<T> {};
#endif // SF_TYPE_REGISTRY_DISABLE

template <typename T> struct is_serializable : all<negation<is_unsupported<T>>, is_registered<T>> {};

} // namespace meta

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_unsupported<T>::value)

#endif // SF_CORE_TYPE_REGISTRY_HPP
