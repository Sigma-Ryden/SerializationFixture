#ifndef SF_BUILT_IN_PAIR_HPP
#define SF_BUILT_IN_PAIR_HPP

#include <type_traits> // true_type, false_type

#include <utility> // pair

#include <SF/Core/Serialization.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_pair : std::false_type {};
template <typename T1, typename T2>
struct is_std_pair<std::pair<T1, T2>> : std::true_type {};

} // namespace meta

} // namespace sf

CONDITIONAL_SERIALIZATION(saveload, pair, ::sf::meta::is_std_pair<T>::value)
{
    archive & pair.first & pair.second;
}

#endif // SF_BUILT_IN_PAIR_HPP
