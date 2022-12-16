#ifndef SIRAF_SUPPORT_LIST_HPP
#define SIRAF_SUPPORT_LIST_HPP

#include <type_traits> // true_type, false_type

#include <list> // list

#include <Siraf/WriteArchive.hpp>
#include <Siraf/ReadArchive.hpp>

#include <Siraf/TypeRegistry.hpp>

#include <Siraf/Utility.hpp>

namespace siraf
{

namespace meta
{

template <typename> struct is_std_list : std::false_type {};
template <typename T, typename Alloc>
struct is_std_list<std::list<T, Alloc>> : std::true_type {};

} // namespace meta

inline namespace library
{

CONDITIONAL_SAVE_SERIALIZABLE_TYPE(list, meta::is_std_list<T>::value)
{
    let::u64 size = list.size();
    archive & size;

    for (auto& item : list)
        archive & item;

    return archive;
}

CONDITIONAL_LOAD_SERIALIZABLE_TYPE(list, meta::is_std_list<T>::value)
{
    let::u64 size;
    archive & size;

    list.resize(size);

    for (auto& item : list)
        archive & item;

    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_std_list<T>::value)

#endif // SIRAF_SUPPORT_LIST_HPP
