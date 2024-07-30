#ifndef SF_BUILT_IN_MAP_HPP
#define SF_BUILT_IN_MAP_HPP

#include <type_traits> // true_type, false_type

#include <map> // map, multimap
#include <unordered_map> // unordered_map, unordered_multimap

#include <utility> // move

#include <SF/Core/TypeCore.hpp>
#include <SF/Core/Serialization.hpp>

#include <SF/Compress.hpp>

// serialization of core map value_type
#include <SF/BuiltIn/pair.hpp>

#define SF_IS_STD_MAP_TYPE_META_GENERIC(map_type)                                                       \
    template <typename> struct is_std_##map_type : std::false_type {};                                  \
    template <typename Key, typename Type, typename Compare, typename Alloc>                            \
    struct is_std_##map_type<std::map_type<Key, Type, Compare, Alloc>> : std::true_type {};

namespace sf
{

namespace meta
{

SF_IS_STD_MAP_TYPE_META_GENERIC(map)
SF_IS_STD_MAP_TYPE_META_GENERIC(unordered_map)
SF_IS_STD_MAP_TYPE_META_GENERIC(multimap)
SF_IS_STD_MAP_TYPE_META_GENERIC(unordered_multimap)

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
          SF_REQUIRE(not meta::is_std_any_unordered_map<T>::value)>
void reserve_unordered(T& ordered, std::size_t size) noexcept { /*pass*/ }

template <class T,
          SF_REQUIRE(meta::is_std_any_unordered_map<T>::value)>
void reserve_unordered(T& unordered, std::size_t size)
{
    unordered.reserve(size);
}

} // namespace detail

} // namespace sf

CONDITIONAL_SERIALIZATION(save, map, ::sf::meta::is_std_any_map<T>::value)
{
    ::sf::let::u64 size = map.size();
    archive & size;

    ::sf::compress::slow(archive, map);
}

CONDITIONAL_SERIALIZATION(load, map, ::sf::meta::is_std_any_map<T>::value)
{
    using key_type   = typename T::key_type;
    using value_type = typename T::mapped_type;

    ::sf::let::u64 size{};
    archive & size;

    map.clear();
    ::sf::detail::reserve_unordered(map, size);

    auto hint = map.begin();
    for (::sf::let::u64 i = 0; i < size; ++i)
    {
        key_type key{};
        value_type value{};

        archive & key & value;

        hint = map.emplace_hint(hint, std::move(key), std::move(value));
    }
}

//clear
#undef SF_IS_STD_MAP_TYPE_META_GENERIC

#endif // SF_BUILT_IN_MAP_HPP
