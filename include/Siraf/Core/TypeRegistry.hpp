#ifndef SIRAF_CORE_TYPE_REGISTRY_HPP
#define SIRAF_CORE_TYPE_REGISTRY_HPP

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

#define CONDITIONAL_TYPE_REGISTRY(...)                                                                  \
    namespace siraf { namespace core {                                                                  \
        template <typename T> struct TypeRegistry<T, SIWHEN(__VA_ARGS__)> : std::true_type {};          \
    }}

// you should use TYPE_REGISTRY before using EXTERN_SERIALIZATION e.t.
#define TYPE_REGISTRY(...)                                                                              \
    namespace siraf { namespace core {                                                                  \
        template <> struct TypeRegistry<__VA_ARGS__> : std::true_type {};                               \
    }}

namespace siraf
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
#ifdef SIRAF_TYPE_REGISTRY_DISABLE
    return true;
#else
    return core::TypeRegistry<T>::value;
#endif // SIRAF_TYPE_REGISTRY_DISABLE
}

} // namespace meta

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_unsupported<T>())

#endif // SIRAF_CORE_TYPE_REGISTRY_HPP
