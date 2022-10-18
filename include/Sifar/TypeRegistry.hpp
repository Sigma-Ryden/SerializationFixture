#ifndef SIFAR_TYPE_REGISTRY_HPP
#define SIFAR_TYPE_REGISTRY_HPP

#include <Sifar/Access.hpp>

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

#define SERIALIZATION_CONDITIONAL_TYPE_REGISTRY(...)                                                    \
    namespace sifar {                                                                                   \
    template <typename T>                                                                               \
    struct TypeRegistry<T,                                                                              \
        SIWHEN(__VA_ARGS__)> : std::true_type {};                                                       \
    }

#define SERIALIZATION_TYPE_REGISTRY(name)                                                               \
    namespace sifar {                                                                                   \
        template <> struct TypeRegistry<name, void> : std::true_type {};                                \
    }

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

SERIALIZATION_CONDITIONAL_TYPE_REGISTRY(meta::is_arithmetic<T>())
SERIALIZATION_CONDITIONAL_TYPE_REGISTRY(meta::is_enum<T>())
SERIALIZATION_CONDITIONAL_TYPE_REGISTRY(meta::is_array<T>())
SERIALIZATION_CONDITIONAL_TYPE_REGISTRY(meta::is_pod_pointer<T>())
SERIALIZATION_CONDITIONAL_TYPE_REGISTRY(meta::is_pointer_to_polymorphic<T>())
SERIALIZATION_CONDITIONAL_TYPE_REGISTRY(meta::is_unsupported<T>())

#endif // SIFAR_TYPE_REGISTRY_HPP
