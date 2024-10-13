#ifndef SF_BUILT_IN_MAP_HPP
#define SF_BUILT_IN_MAP_HPP

#include <cstdint> // uint64_t

#include <type_traits> // true_type, false_type

#include <map> // map, multimap
#include <unordered_map> // unordered_map, unordered_multimap

#include <utility> // move

#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/Compress.hpp>

// serialization of core map value_type
#include <SerializationFixture/BuiltIn/pair.hpp>

#define SF_IS_STD_MAP_TYPE_META_GENERIC(map_type)                                                       \
    template <typename> struct is_std_##map_type : std::false_type {};                                  \
    template <typename KeyType, typename ValueType, typename ComparatorType, typename AllocatorType>    \
    struct is_std_##map_type<std::map_type<KeyType, ValueType, ComparatorType, AllocatorType>> : std::true_type {};

namespace sf
{

namespace meta
{

SF_IS_STD_MAP_TYPE_META_GENERIC(map)
SF_IS_STD_MAP_TYPE_META_GENERIC(unordered_map)
SF_IS_STD_MAP_TYPE_META_GENERIC(multimap)
SF_IS_STD_MAP_TYPE_META_GENERIC(unordered_multimap)

template <class StdMapType> struct is_std_any_unordered_map
    : one<is_std_unordered_map<StdMapType>,
          is_std_unordered_multimap<StdMapType>> {};

template <class StdMapType> struct is_std_any_map
    : one<is_std_map<StdMapType>,
          is_std_multimap<StdMapType>,
          is_std_any_unordered_map<StdMapType>> {};

} // namespace meta

namespace detail
{

template <class StdMapType,
          SF_REQUIRES(meta::negation<meta::is_std_any_unordered_map<StdMapType>>::value)>
void reserve_unordered(StdMapType&, std::size_t) noexcept { /*pass*/ }

template <class StdMapType,
          SF_REQUIRES(meta::is_std_any_unordered_map<StdMapType>::value)>
void reserve_unordered(StdMapType& unordered, std::size_t size)
{
    unordered.reserve(size);
}

} // namespace detail

} // namespace sf

CONDITIONAL_SERIALIZATION(save, map, ::sf::meta::is_std_any_map<S>::value)
{
    std::uint64_t size = map.size();
    archive & size;

    ::sf::compress::slow(archive, map);
}

CONDITIONAL_SERIALIZATION(load, map, ::sf::meta::is_std_any_map<S>::value)
{
    using key_type   = typename S::key_type;
    using value_type = typename S::mapped_type;

    std::uint64_t size{};
    archive & size;

    map.clear();
    ::sf::detail::reserve_unordered(map, size);

    auto hint = map.begin();
    for (std::uint64_t i = 0; i < size; ++i)
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
