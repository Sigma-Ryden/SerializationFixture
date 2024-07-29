#ifndef SF_CORE_TYPE_REGISTRY_HPP
#define SF_CORE_TYPE_REGISTRY_HPP

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

#define CONDITIONAL_TYPE_REGISTRY(...)                                                                  \
    template <typename T>                                                                               \
    struct xxsf_registry<T, typename std::enable_if<__VA_ARGS__>::type> : std::true_type {};

// you should use TYPE_REGISTRY before using EXTERN_SERIALIZATION e.t.
#define TYPE_REGISTRY(...)                                                                              \
    template <> struct xxsf_registry<__VA_ARGS__> : std::true_type {};

template <typename T, typename enable = void>
struct xxsf_registry : std::false_type {};

namespace sf
{

namespace meta
{

template <typename T> struct is_registered : ::xxsf_registry<T> {};

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

CONDITIONAL_TYPE_REGISTRY(::sf::meta::is_unsupported<T>::value)

#endif // SF_CORE_TYPE_REGISTRY_HPP
