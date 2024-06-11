#ifndef SF_BUILT_IN_LIST_HPP
#define SF_BUILT_IN_LIST_HPP

#include <type_traits> // true_type, false_type

#include <list> // list

#include <SF/Core/TypeRegistry.hpp>
#include <SF/Core/TypeCore.hpp>

#include <SF/Compress.hpp>
#include <SF/ExternSerialization.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_list : std::false_type {};
template <typename T, typename Alloc>
struct is_std_list<std::list<T, Alloc>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, list, meta::is_std_list<T>::value)
{
    let::u64 size = list.size();
    archive & size;

    compress::slow(archive, list);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, list, meta::is_std_list<T>::value)
{
    let::u64 size{};
    archive & size;

    list.resize(size);
    compress::slow(archive, list);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_list<T>::value)

#endif // SF_BUILT_IN_LIST_HPP
