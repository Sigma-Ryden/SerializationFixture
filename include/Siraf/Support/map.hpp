#ifndef SIRAF_SUPPORT_MAP_HPP
#define SIRAF_SUPPORT_MAP_HPP

#include <type_traits> // true_type, false_type

#include <map> // map, multimap
#include <unordered_map> // unordered_map, unordered_multimap

#include <utility> // move

#include <Siraf/Core/TypeRegistry.hpp>
#include <Siraf/Core/TypeCore.hpp>

#include <Siraf/Compress.hpp>
#include <Siraf/ExternSerialization.hpp>

// serialization of core map value_type
#include <Siraf/Support/pair.hpp>

#define _SIRAF_IS_STD_MAP_TYPE_META_GENERIC(map_type)                                                   \
    template <typename> struct is_std_##map_type : std::false_type {};                                  \
    template <typename Key, typename Type, typename Compare, typename Alloc>                            \
    struct is_std_##map_type<std::map_type<Key, Type, Compare, Alloc>> : std::true_type {};

namespace siraf
{

namespace meta
{

_SIRAF_IS_STD_MAP_TYPE_META_GENERIC(map)
_SIRAF_IS_STD_MAP_TYPE_META_GENERIC(unordered_map)
_SIRAF_IS_STD_MAP_TYPE_META_GENERIC(multimap)
_SIRAF_IS_STD_MAP_TYPE_META_GENERIC(unordered_multimap)

template <class T> constexpr bool is_std_any_unordered_map() noexcept
{
    return is_std_unordered_map<T>::value
        or is_std_unordered_multimap<T>::value;
}

template <class T> constexpr bool is_std_any_map() noexcept
{
    return is_std_map<T>::value
        or is_std_multimap<T>::value
        or is_std_any_unordered_map<T>();
}

} // namespace meta

namespace detail
{

template <class T,
          SIREQUIRE(not meta::is_std_any_unordered_map<T>())>
void reserve_unordered(T& ordered, std::size_t size) noexcept { /*pass*/ }

template <class T,
          SIREQUIRE(meta::is_std_any_unordered_map<T>())>
void reserve_unordered(T& unordered, std::size_t size)
{
    unordered.reserve(size);
}

} // namespace detail

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, map, meta::is_std_any_map<T>())
{
    let::u64 size = map.size();
    archive & size;

    compress::slow(archive, map);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, map, meta::is_std_any_map<T>())
{
    using key   = typename T::key_type;
    using value = typename T::mapped_type;

    let::u64 size{};
    archive & size;

    map.clear();
    detail::reserve_unordered(map, size);

    auto hint = map.begin();
    for (let::u64 i = 0; i < size; ++i)
    {
        std::pair<key, value> item;
        archive & item;

        hint = map.emplace_hint(hint, std::move(item));
    }

    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_any_map<T>())

//clear
#undef _SIRAF_IS_STD_MAP_TYPE_META_GENERIC

#endif // SIRAF_SUPPORT_MAP_HPP
