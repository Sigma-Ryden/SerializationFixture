#ifndef SF_BUILT_IN_MAP_HPP
#define SF_BUILT_IN_MAP_HPP

#include <type_traits> // true_type, false_type

#include <map> // map, multimap
#include <unordered_map> // unordered_map, unordered_multimap

#include <utility> // move

#include <SF/Core/TypeRegistry.hpp>
#include <SF/Core/TypeCore.hpp>

#include <SF/Compress.hpp>
#include <SF/ExternSerialization.hpp>

// serialization of core map value_type
#include <SF/BuiltIn/pair.hpp>

#define _SF_IS_STD_MAP_TYPE_META_GENERIC(map_type)                                                      \
    template <typename> struct is_std_##map_type : std::false_type {};                                  \
    template <typename Key, typename Type, typename Compare, typename Alloc>                            \
    struct is_std_##map_type<std::map_type<Key, Type, Compare, Alloc>> : std::true_type {};

namespace sf
{

namespace meta
{

_SF_IS_STD_MAP_TYPE_META_GENERIC(map)
_SF_IS_STD_MAP_TYPE_META_GENERIC(unordered_map)
_SF_IS_STD_MAP_TYPE_META_GENERIC(multimap)
_SF_IS_STD_MAP_TYPE_META_GENERIC(unordered_multimap)

template <class T> struct is_std_any_unordered_map
    : one<is_std_unordered_map<T>,
          is_std_unordered_multimap<T>> {};

template <class T> struct is_std_any_map
    : one<is_std_map<T>,
          is_std_multimap<T>,
          is_std_any_unordered_map<T>> {};

} // namespace meta

namespace detail
{

template <class T,
          SFREQUIRE(not meta::is_std_any_unordered_map<T>::value)>
void reserve_unordered(T& ordered, std::size_t size) noexcept { /*pass*/ }

template <class T,
          SFREQUIRE(meta::is_std_any_unordered_map<T>::value)>
void reserve_unordered(T& unordered, std::size_t size)
{
    unordered.reserve(size);
}

} // namespace detail

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, map, meta::is_std_any_map<T>::value)
{
    let::u64 size = map.size();
    archive & size;

    compress::slow(archive, map);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, map, meta::is_std_any_map<T>::value)
{
    using key_type   = typename T::key_type;
    using value_type = typename T::mapped_type;

    let::u64 size{};
    archive & size;

    map.clear();
    detail::reserve_unordered(map, size);

    auto hint = map.begin();
    for (let::u64 i = 0; i < size; ++i)
    {
        key_type key{};
        value_type value{};

        archive & key & value;

        hint = map.emplace_hint(hint, std::move(key), std::move(value));
    }

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_any_map<T>::value)

//clear
#undef _SF_IS_STD_MAP_TYPE_META_GENERIC

#endif // SF_BUILT_IN_MAP_HPP
