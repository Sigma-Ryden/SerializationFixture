#ifndef EIGHTSER_BUILT_IN_MAP_HPP
#define EIGHTSER_BUILT_IN_MAP_HPP

#include <cstdint> // uint64_t

#include <type_traits> // true_type, false_type

#include <map> // map, multimap
#include <unordered_map> // unordered_map, unordered_multimap

#include <utility> // move

#include <Eightser/Core/Serialization.hpp>

#include <Eightser/Compress.hpp>

// serialization of core map value_type
#include <Eightser/BuiltIn/pair.hpp>

#define EIGHTSER_IS_STD_MAP_TYPE_META_GENERIC(map_type) \
    template <typename> struct xxeightser_is_std_##map_type : std::false_type {}; \
    template <typename KeyType, typename ValueType, typename ComparatorType, typename AllocatorType> \
    struct xxeightser_is_std_##map_type<std::map_type<KeyType, ValueType, ComparatorType, AllocatorType>> : std::true_type {};


EIGHTSER_IS_STD_MAP_TYPE_META_GENERIC(map)
EIGHTSER_IS_STD_MAP_TYPE_META_GENERIC(unordered_map)
EIGHTSER_IS_STD_MAP_TYPE_META_GENERIC(multimap)
EIGHTSER_IS_STD_MAP_TYPE_META_GENERIC(unordered_multimap)

template <class StdMapType> struct xxeightser_is_std_any_unordered_map
    : std::disjunction<::xxeightser_is_std_unordered_map<StdMapType>,
                       ::xxeightser_is_std_unordered_multimap<StdMapType>> {};

template <class StdMapType> struct xxeightser_is_std_any_map
    : std::disjunction<::xxeightser_is_std_map<StdMapType>,
                       ::xxeightser_is_std_multimap<StdMapType>,
                       ::xxeightser_is_std_any_unordered_map<StdMapType>> {};

namespace eightser
{

namespace detail
{

template <class StdMapType,
          EIGHTSER_REQUIRES(std::negation<::xxeightser_is_std_any_unordered_map<StdMapType>>::value)>
void reserve_unordered(StdMapType&, std::size_t) noexcept { /*pass*/ }

template <class StdMapType,
          EIGHTSER_REQUIRES(::xxeightser_is_std_any_unordered_map<StdMapType>::value)>
void reserve_unordered(StdMapType& unordered, std::size_t size)
{
    unordered.reserve(size);
}

} // namespace detail

} // namespace eightser

CONDITIONAL_SERIALIZABLE_DECLARATION(::xxeightser_is_std_any_map<S>::value)
SERIALIZABLE_DECLARATION_INIT()

CONDITIONAL_SERIALIZABLE(save, map, ::xxeightser_is_std_any_map<S>::value)
    SERIALIZATION
    (
        std::uint64_t size = map.size();
        archive & size;

        ::eightser::compress::slow(archive, map);
    )
SERIALIZABLE_INIT()

CONDITIONAL_SERIALIZABLE(load, map, ::xxeightser_is_std_any_map<S>::value)
    SERIALIZATION
    (
        std::uint64_t size{};
        archive & size;

        map.clear();
        ::eightser::detail::reserve_unordered(map, size);

        auto hint = map.begin();
        for (std::uint64_t i = 0; i < size; ++i)
        {
            typename S::key_type key{};
            typename S::mapped_type value{};

            archive & key & value;

            hint = map.emplace_hint(hint, std::move(key), std::move(value));
        }
    )
SERIALIZABLE_INIT()

//clear
#undef EIGHTSER_IS_STD_MAP_TYPE_META_GENERIC

#endif // EIGHTSER_BUILT_IN_MAP_HPP
