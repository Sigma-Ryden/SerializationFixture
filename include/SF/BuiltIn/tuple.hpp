#ifndef SF_BUILT_IN_TUPLE_HPP
#define SF_BUILT_IN_TUPLE_HPP

#include <type_traits> // true_type, false_type

#include <tuple> // tuple

#include <SF/Core/Serialization.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_tuple : std::false_type {};
template <typename... Tn>
struct is_std_tuple<std::tuple<Tn...>> : std::true_type {};

} // namespace meta

namespace detail
{

template <class Archive, class T, std::size_t... I,
          SF_REQUIRE(meta::is_std_tuple<T>::value)>
void expand(Archive& archive, T& tuple, meta::index_sequence<I...>)
{
    archive(std::get<I>(tuple)...);
}

} // namespace detail

} // namespace sf

CONDITIONAL_SERIALIZATION(saveload, tuple, ::sf::meta::is_std_tuple<T>::value)
{
    constexpr auto size = std::tuple_size<T>::value;
    ::sf::detail::expand(archive, tuple, ::sf::meta::make_index_sequence<size>{});
}

#endif // SF_BUILT_IN_TUPLE_HPP
