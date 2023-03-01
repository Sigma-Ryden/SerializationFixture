#ifndef SIRAF_SUPPORT_ATOMIC_HPP
#define SIRAF_SUPPORT_ATOMIC_HPP

#include <type_traits> // true_type, false_type

#include <atomic> // atomic

#include <Siraf/ExternSerialization.hpp>
#include <Siraf/TypeRegistry.hpp>

namespace siraf
{

namespace meta
{

template <typename> struct is_std_atomic : std::false_type {};
template <typename T>
struct is_std_atomic<std::atomic<T>> : std::true_type {};

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
    using object_type = typename T::value_type;

    object_type object;
    archive & object;

    atomic.store(object);

    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_atomic<T>::value)

#endif // SIRAF_SUPPORT_ATOMIC_HPP
