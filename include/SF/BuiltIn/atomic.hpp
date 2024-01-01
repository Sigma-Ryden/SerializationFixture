#ifndef SF_BUILT_IN_ATOMIC_HPP
#define SF_BUILT_IN_ATOMIC_HPP

#include <type_traits> // true_type, false_type

#include <atomic> // atomic

#include <SF/Core/TypeRegistry.hpp>
#include <SF/ExternSerialization.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_atomic : std::false_type {};
template <typename T> struct is_std_atomic<std::atomic<T>> : std::true_type {};

template <typename T> struct atomic_trait;
template <typename T> struct atomic_trait<std::atomic<T>> { using value_type = T; };

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, atomic, meta::is_std_atomic<T>::value)
{
    auto object = atomic.load();
    archive & object;

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, atomic, meta::is_std_atomic<T>::value)
{
    using object_type = typename meta::atomic_trait<T>::value_type;

    object_type object{};
    archive & object;

    atomic.store(object);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_atomic<T>::value)

#endif // SF_BUILT_IN_ATOMIC_HPP
