#ifndef SIFAR_SUPPORT_LIST_HPP
#define SIFAR_SUPPORT_LIST_HPP

#include <type_traits> // true_type, false_type

#include <list> // list

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>

#include <Sifar/TypeRegistry.hpp>

#include <Sifar/Utility.hpp>

namespace sifar
{

namespace meta
{

template <typename> struct is_std_list : std::false_type {};
template <typename T, typename Alloc>
struct is_std_list<std::list<T, Alloc>> : std::true_type {};

} // namespace meta

namespace library
{

SERIALIZATION_SAVE_DATA(list, meta::is_std_list<T>::value)
{
    let::u64 size = list.size();

    archive & size;
    for (const auto& item : list)
        archive & item;

    return archive;
}

SERIALIZATION_LOAD_DATA(list, meta::is_std_list<T>::value)
{
    let::u64 size;
    archive & size;

    list.resize(size);

    for (auto& item : list)
        archive & item;

    return archive;
}

} // namespace library

} // namespace sifar

SERIALIZATION_TYPE_REGISTRY_IF(meta::is_std_list<T>::value)

#endif // SIFAR_SUPPORT_LIST_HPP
