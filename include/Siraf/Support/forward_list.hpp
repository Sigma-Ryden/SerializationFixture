#ifndef SIRAF_SUPPORT_FORWARD_LIST_HPP
#define SIRAF_SUPPORT_FORWARD_LIST_HPP

#include <type_traits> // true_type, false_type

#include <forward_list> // forward_list

#include <Siraf/Core/TypeRegistry.hpp>
#include <Siraf/Core/TypeCore.hpp>

#include <Siraf/Compress.hpp>
#include <Siraf/ExternSerialization.hpp>

namespace siraf
{

namespace meta
{

template <typename> struct is_std_forward_list : std::false_type {};
template <typename T, typename Alloc>
struct is_std_forward_list<std::forward_list<T, Alloc>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, forward_list, meta::is_std_forward_list<T>::value)
{
    let::u64 size = std::distance(forward_list.begin(), forward_list.end());
    archive & size;

    compress::slow(archive, forward_list);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, forward_list, meta::is_std_forward_list<T>::value)
{
    let::u64 size{};
    archive & size;

    forward_list.resize(size);
    compress::slow(archive, forward_list);

    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_forward_list<T>::value)

#endif // SIRAF_SUPPORT_FORWARD_LIST_HPP
