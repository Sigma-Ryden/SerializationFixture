#ifndef SIFAR_SUPPORT_SET_HPP
#define SIFAR_SUPPORT_SET_HPP

#include <type_traits> // true_type, false_type

#include <set> // set, multiset
#include <utility> // move

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>

#include <Sifar/TypeRegistry.hpp>

#include <Sifar/Utility.hpp>

namespace sifar
{

namespace meta
{

template <typename> struct is_std_set : std::false_type {};
template <typename Key, typename Compare, typename Alloc>
struct is_std_set<std::set<Key, Compare, Alloc>> : std::true_type {};

template <typename> struct is_std_multiset : std::false_type {};
template <typename Key, typename Compare, typename Alloc>
struct is_std_multiset<std::multiset<Key, Compare, Alloc>> : std::true_type {};

} // namespace meta

namespace library
{

SERIALIZATION_SAVE_DATA(set, meta::is_std_set<T>::value or
                             meta::is_std_multiset<T>::value)
{
    let::u64 size = set.size();
    archive & size;

    for (const auto& item : set)
        archive & item;

    return archive;
}

SERIALIZATION_LOAD_DATA(set, meta::is_std_set<T>::value or
                             meta::is_std_multiset<T>::value)
{
    using value_type = typename T::value_type;

    let::u64 size;
    archive & size;

    set.clear();

    auto hint = set.begin();
    for (let::u64 i = 0; i < size; ++i)
    {
        value_type item{}; // temp
        archive & item;

        hint = set.emplace_hint(hint, std::move(item));
    }

    return archive;
}

} // namespace library

} // namespace sifar

SERIALIZATION_CONDITIONAL_TYPE_REGISTRY(meta::is_std_set<T>::value)
SERIALIZATION_CONDITIONAL_TYPE_REGISTRY(meta::is_std_multiset<T>::value)

#endif // SIFAR_SUPPORT_SET_HPP
