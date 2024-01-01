#ifndef SF_BUILT_IN_UNIQUE_PTR_HPP
#define SF_BUILT_IN_UNIQUE_PTR_HPP

#include <type_traits> // true_type, false_type

#include <memory> // unique_ptr

#include <SF/Core/TypeRegistry.hpp>
#include <SF/Core/Memory.hpp>

#include <SF/ExternSerialization.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_unique_ptr : std::false_type {};
template <typename T, typename Deleter>
struct is_std_unique_ptr<std::unique_ptr<T, Deleter>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, unique_ptr, meta::is_std_unique_ptr<T>::value)
{
    auto data = unique_ptr.get();
    archive & data;

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, unique_ptr, meta::is_std_unique_ptr<T>::value)
{
    using item_type = typename Memory::ptr_trait<T>::item;

    item_type* data = nullptr;
    archive & data;

    unique_ptr.reset(data);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_unique_ptr<T>::value)

#endif // SF_BUILT_IN_UNIQUE_PTR_HPP
