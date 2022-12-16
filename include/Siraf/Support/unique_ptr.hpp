#ifndef SIRAF_SUPPORT_UNIQUE_PTR_HPP
#define SIRAF_SUPPORT_UNIQUE_PTR_HPP

#include <type_traits> // true_type, false_type

#include <memory> // unique_ptr

#include <Siraf/WriteArchive.hpp>
#include <Siraf/ReadArchive.hpp>

#include <Siraf/TypeRegistry.hpp>
#include <Siraf/Memory/Memory.hpp>

namespace siraf
{

namespace meta
{

template <typename> struct is_std_unique_ptr : std::false_type {};
template <typename T, typename Deleter>
struct is_std_unique_ptr<std::unique_ptr<T, Deleter>> : std::true_type {};

} // namespace meta

inline namespace library
{

CONDITIONAL_SAVE_SERIALIZABLE_TYPE(unique_ptr, meta::is_std_unique_ptr<T>::value)
{
    auto data = unique_ptr.get();
    archive & data;

    return archive;
}

CONDITIONAL_LOAD_SERIALIZABLE_TYPE(unique_ptr, meta::is_std_unique_ptr<T>::value)
{
    using item_type = typename memory::ptr_trait<T>::item_type;

    item_type* data = nullptr;
    archive & data;

    unique_ptr.reset(data);

    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_std_unique_ptr<T>::value)

#endif // SIRAF_SUPPORT_UNIQUE_PTR_HPP
