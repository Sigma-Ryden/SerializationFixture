#ifndef SF_BUILT_IN_BITSET_HPP
#define SF_BUILT_IN_BITSET_HPP

#include <type_traits> // true_type, false_type

#include <bitset> // bitset

#include <SF/Core/Serialization.hpp>

// default array for bitset convertion
#include <SF/BuiltIn/string.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_bitset : std::false_type {};
template <std::size_t N> struct is_std_bitset<std::bitset<N>> : std::true_type {};

} // namespace meta

} // namespace sf

// slow impl
CONDITIONAL_SERIALIZATION(save, bitset, ::sf::meta::is_std_bitset<T>::value)
{
    auto data = bitset.to_string();
    archive & data;
}

CONDITIONAL_SERIALIZATION(load, bitset, ::sf::meta::is_std_bitset<T>::value)
{
    std::string data;
    archive & data;

    bitset = T(data);
}

#endif // SF_BUILT_IN_BITSET_HPP
