#ifndef SIFAR_TYPE_REGISTRY_HPP
#define SIFAR_TYPE_REGISTRY_HPP

#include <Sifar/Access.hpp>

#include <Sifar/Detail/Meta.hpp>

#define SERIALIZATION_TYPE_REGISTRY_IF(...)                                                             \
    namespace sifar {                                                                                   \
    template <typename T>                                                                               \
    struct TypeRegistry<T,                                                                              \
        ::sifar::meta::when<(bool)(__VA_ARGS__)>> : std::true_type {};                                  \
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

SERIALIZATION_TYPE_REGISTRY_IF(Access::is_save_load_class<T>())
SERIALIZATION_TYPE_REGISTRY_IF(meta::is_arithmetic<T>())
SERIALIZATION_TYPE_REGISTRY_IF(meta::is_enum<T>())
SERIALIZATION_TYPE_REGISTRY_IF(meta::is_array<T>())
SERIALIZATION_TYPE_REGISTRY_IF(meta::is_pod_pointer<T>())
SERIALIZATION_TYPE_REGISTRY_IF(meta::is_pointer_to_polymorphic<T>())
SERIALIZATION_TYPE_REGISTRY_IF(meta::is_unsupported<T>())

#endif // SIFAR_TYPE_REGISTRY_HPP
