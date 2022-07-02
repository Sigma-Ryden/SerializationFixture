#ifndef SERIALIZATION_TYPE_REGISTRY_HPP
#define SERIALIZATION_TYPE_REGISTRY_HPP

#include <Serialization/Access.hpp>
#include <Serialization/Ref.hpp>

#include <Serialization/Detail/Meta.hpp>

#define SERIALIZATION_TYPE_REGISTRY_IF(...)                                                             \
    namespace serialization {                                                                           \
    template <typename T>                                                                               \
    struct TypeRegistry<T,                                                                              \
        serialization::meta::when<(bool)(__VA_ARGS__)>> : std::true_type {};                            \
    }

#define SERIALIZATION_TYPE_REGISTRY(name)                                                               \
    namespace serialization {                                                                           \
        template <> struct TypeRegistry<name, void> : std::true_type {};                                \
    }

namespace serialization
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

} // namespace serialization

SERIALIZATION_TYPE_REGISTRY_IF(Access::is_save_load_class<T>())
SERIALIZATION_TYPE_REGISTRY_IF(meta::is_arithmetic<T>())
SERIALIZATION_TYPE_REGISTRY_IF(meta::is_enum<T>())
SERIALIZATION_TYPE_REGISTRY_IF(meta::is_array<T>())
SERIALIZATION_TYPE_REGISTRY_IF(meta::is_ref<T>())
SERIALIZATION_TYPE_REGISTRY_IF(meta::is_pod_pointer<T>())
SERIALIZATION_TYPE_REGISTRY_IF(meta::is_polymorphic_pointer<T>())
SERIALIZATION_TYPE_REGISTRY_IF(meta::is_unsupported<T>())

#endif // SERIALIZATION_TYPE_REGISTRY_HPP
