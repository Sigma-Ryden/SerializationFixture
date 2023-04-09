#ifndef SF_CORE_TYPE_REGISTRY_HPP
#define SF_CORE_TYPE_REGISTRY_HPP

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

#define CONDITIONAL_TYPE_REGISTRY(...)                                                                  \
    namespace sf { namespace core {                                                                     \
        template <typename T> struct TypeRegistry<T, SIWHEN(__VA_ARGS__)> : std::true_type {};          \
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

template <typename T> constexpr bool is_registered() noexcept
{
    return core::TypeRegistry<T>::value;
}

// use this function only for extern type registry check
template <typename T> constexpr bool is_registered_extern() noexcept
{
#ifdef SF_TYPE_REGISTRY_DISABLE
    return true;
#else
    return is_registered<T>();
#endif // SF_TYPE_REGISTRY_DISABLE
}

} // namespace meta

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_unsupported<T>())

#endif // SF_CORE_TYPE_REGISTRY_HPP
