#ifndef SF_BUILT_IN_SET_HPP
#define SF_BUILT_IN_SET_HPP

#include <type_traits> // true_type, false_type

#include <set> // set, multiset
#include <unordered_set> // unordered_set, unordered_multiset

#include <utility> // move

#include <cstdint>
#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/Compress.hpp>

#define SF_IS_STD_SET_TYPE_META_GENERIC(set_type)                                                       \
    template <typename> struct is_std_##set_type : std::false_type {};                                  \
    template <typename KeyType, typename ComparatorType, typename AllocatorType>                        \
    struct is_std_##set_type<std::set_type<KeyType, ComparatorType, AllocatorType>> : std::true_type {};

namespace sf
{

namespace meta
{

SF_IS_STD_SET_TYPE_META_GENERIC(set)
SF_IS_STD_SET_TYPE_META_GENERIC(unordered_set)
SF_IS_STD_SET_TYPE_META_GENERIC(multiset)
SF_IS_STD_SET_TYPE_META_GENERIC(unordered_multiset)

template <class StdSetType> struct is_std_any_unordered_set
    : one<is_std_unordered_set<StdSetType>,
          is_std_unordered_multiset<StdSetType>> {};

template <class StdSetType> struct is_std_any_set
    : one<is_std_set<StdSetType>,
          is_std_multiset<StdSetType>,
          is_std_any_unordered_set<StdSetType>> {};

} // namespace meta

namespace detail
{

template <class StdSetType,
          SF_REQUIRES(meta::negation<meta::is_std_any_unordered_set<StdSetType>>::value)>
void reserve_unordered(StdSetType&, std::size_t) noexcept { /*pass*/ }

template <class StdSetType,
          SF_REQUIRES(meta::is_std_any_unordered_set<StdSetType>::value)>
void reserve_unordered(StdSetType& unordered, std::size_t size)
{
    unordered.reserve(size);
}

} // namespace detail

} // namespace sf

CONDITIONAL_SERIALIZATION(save, set, ::sf::meta::is_std_any_set<S>::value)
{
    std::uint64_t size = set.size();
    archive & size;

    ::sf::compress::slow(archive, set);
}

CONDITIONAL_SERIALIZATION(load, set, ::sf::meta::is_std_any_set<S>::value)
{
    using value_type = typename S::value_type;

    std::uint64_t size{};
    archive & size;

    set.clear();
    ::sf::detail::reserve_unordered(set, size);

    auto hint = set.begin();
    for (std::uint64_t i = 0; i < size; ++i)
    {
        value_type item{}; // temp
        archive & item;

        hint = set.emplace_hint(hint, std::move(item));
    }
}

// clean up
#undef SF_IS_STD_SET_TYPE_META_GENERIC

#endif // SF_BUILT_IN_SET_HPP
