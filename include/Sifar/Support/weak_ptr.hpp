#ifndef SIFAR_SUPPORT_WEAK_PTR_HPP
#define SIFAR_SUPPORT_WEAK_PTR_HPP

#include <type_traits> // true_type, false_type

#include <memory> // weak_ptr

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>

#include <Sifar/TypeRegistry.hpp>

// serialization of shared_ptr
#include <Sifar/Support/shared_ptr.hpp>

namespace sifar
{

namespace meta
{

template <typename> struct is_std_weak_ptr : std::false_type {};
template <typename T>
struct is_std_weak_ptr<std::weak_ptr<T>> : std::true_type {};

} // namespace meta

inline namespace library
{

CONDITIONAL_SAVE_SERIALIZABLE_TYPE(weak_ptr, meta::is_std_weak_ptr<T>::value)
{
    auto sptr = weak_ptr.lock();
    archive & sptr;

    return archive;
}

CONDITIONAL_LOAD_SERIALIZABLE_TYPE(weak_ptr, meta::is_std_weak_ptr<T>::value)
{
    using element_type = typename T::element_type;

    std::shared_ptr<element_type> sptr;
    archive & sptr;

    weak_ptr = sptr;

    return archive;
}

} // inline namespace library

} // namespace sifar

CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_std_weak_ptr<T>::value)

#endif // SIFAR_SUPPORT_WEAK_PTR_HPP
