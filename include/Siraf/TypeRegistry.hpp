#ifndef SIRAF_TYPE_REGISTRY_HPP
#define SIRAF_TYPE_REGISTRY_HPP

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

#define CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(...)                                                     \
    namespace siraf {                                                                                   \
        template <typename T> struct TypeRegistry<T, SIWHEN(__VA_ARGS__)> : std::true_type {};          \
    }

#define REGISTRY_SERIALIZABLE_TYPE(...)                                                                 \
    namespace siraf {                                                                                   \
        template <> struct TypeRegistry<__VA_ARGS__, void> : std::true_type {};                         \
    }

#ifdef SIRAF_SMART
    #define CONDITIONAL_REGISTRY_TYPE(...) CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(__VA_ARGS__)
    #define REGISTRY_TYPE(...) REGISTRY_SERIALIZABLE_TYPE(__VA_ARGS__)
#endif // SIRAF_SMART

namespace siraf
{

template <typename T, typename enable = void>
struct TypeRegistry : std::false_type {};

namespace meta
{

template <typename T> constexpr bool is_registered() noexcept
{
    return TypeRegistry<T>::value;
}

} // namespace meta

} // namespace siraf

CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_arithmetic<T>())
CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_enum<T>())
CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_array<T>())
CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_serializable_raw_pointer<T>())
CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_unsupported<T>())

#endif // SIRAF_TYPE_REGISTRY_HPP
