#ifndef SIRAF_SUPPORT_WEAK_PTR_HPP
#define SIRAF_SUPPORT_WEAK_PTR_HPP

#include <type_traits> // true_type, false_type

#include <memory> // weak_ptr

#include <Siraf/ExternSerialization.hpp>
#include <Siraf/TypeRegistry.hpp>

#include <Siraf/Memory/Memory.hpp>

// serialization of shared_ptr
#include <Siraf/Support/shared_ptr.hpp>

namespace siraf
{

namespace meta
{

template <typename> struct is_std_weak_ptr : std::false_type {};
template <typename T>
struct is_std_weak_ptr<std::weak_ptr<T>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, weak_ptr, meta::is_std_weak_ptr<T>::value)
{
    auto sptr = weak_ptr.lock();
    archive & sptr;

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, weak_ptr, meta::is_std_weak_ptr<T>::value)
{
    using item_type = typename memory::ptr_trait<T>::item;

    std::shared_ptr<item_type> sptr;
    archive & sptr;

    weak_ptr = sptr;

    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_weak_ptr<T>::value)

#endif // SIRAF_SUPPORT_WEAK_PTR_HPP
