#ifndef SF_BUILT_IN_SET_HPP
#define SF_BUILT_IN_SET_HPP

#include <type_traits> // true_type, false_type

#include <set> // set, multiset
#include <unordered_set> // unordered_set, unordered_multiset

#include <utility> // move

#include <SF/Core/TypeCore.hpp>
#include <SF/Core/Serialization.hpp>

#include <SF/Compress.hpp>

#define SF_IS_STD_SET_TYPE_META_GENERIC(set_type)                                                       \
    template <typename> struct is_std_##set_type : std::false_type {};                                  \
    template <typename Key, typename Compare, typename Alloc>                                           \
    struct is_std_##set_type<std::set_type<Key, Compare, Alloc>> : std::true_type {};

namespace sf
{

namespace meta
{

SF_IS_STD_SET_TYPE_META_GENERIC(set)
SF_IS_STD_SET_TYPE_META_GENERIC(unordered_set)
SF_IS_STD_SET_TYPE_META_GENERIC(multiset)
SF_IS_STD_SET_TYPE_META_GENERIC(unordered_multiset)

template <class T> struct is_std_any_unordered_set
    : one<is_std_unordered_set<T>,
          is_std_unordered_multiset<T>> {};

template <class T> struct is_std_any_set
    : one<is_std_set<T>,
          is_std_multiset<T>,
          is_std_any_unordered_set<T>> {};

} // namespace meta

namespace detail
{

template <class T,
          SF_REQUIRE(not meta::is_std_any_unordered_set<T>::value)>
void reserve_unordered(T& ordered, std::size_t size) noexcept { /*pass*/ }

template <class T,
          SF_REQUIRE(meta::is_std_any_unordered_set<T>::value)>
void reserve_unordered(T& unordered, std::size_t size)
{
    unordered.reserve(size);
}

} // namespace detail

} // namespace sf

CONDITIONAL_SERIALIZATION(save, set, ::sf::meta::is_std_any_set<T>::value)
{
    ::sf::let::u64 size = set.size();
    archive & size;

    ::sf::compress::slow(archive, set);
}

CONDITIONAL_SERIALIZATION(load, set, ::sf::meta::is_std_any_set<T>::value)
{
    using value_type = typename T::value_type;

    ::sf::let::u64 size{};
    archive & size;

    set.clear();
    ::sf::detail::reserve_unordered(set, size);

    auto hint = set.begin();
    for (::sf::let::u64 i = 0; i < size; ++i)
    {
        value_type item{}; // temp
        archive & item;

        hint = set.emplace_hint(hint, std::move(item));
    }
}

// clean up
#undef SF_IS_STD_SET_TYPE_META_GENERIC

#endif // SF_BUILT_IN_SET_HPP
