#ifndef SIFAR_TYPE_REGISTRY_HPP
#define SIFAR_TYPE_REGISTRY_HPP

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

#define CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(...)                                                     \
    namespace sifar {                                                                                   \
        template <typename T> struct TypeRegistry<T, SIWHEN(__VA_ARGS__)> : std::true_type {};          \
    }

#define REGISTRY_SERIALIZABLE_TYPE(...)                                                                 \
    namespace sifar {                                                                                   \
        template <> struct TypeRegistry<__VA_ARGS__, void> : std::true_type {};                         \
    }

#ifdef SIFAR_SMART
    #define CONDITIONAL_REGISTRY_TYPE(...) CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(__VA_ARGS__)
    #define REGISTRY_TYPE(...) REGISTRY_SERIALIZABLE_TYPE(__VA_ARGS__)
#endif // SIFAR_SMART

namespace sifar
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

} // namespace sifar

CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_arithmetic<T>())
CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_enum<T>())
CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_array<T>())
CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_pod_pointer<T>())
CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_pointer_to_polymorphic<T>())
CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_unsupported<T>())

#endif // SIFAR_TYPE_REGISTRY_HPP
