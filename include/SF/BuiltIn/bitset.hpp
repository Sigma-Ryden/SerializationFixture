#ifndef SF_BUILT_IN_BITSET_HPP
#define SF_BUILT_IN_BITSET_HPP

#include <type_traits> // true_type, false_type

#include <bitset> // bitset

#include <SF/Core/TypeRegistry.hpp>
#include <SF/ExternSerialization.hpp>

// default array for bitset convertion
#include <SF/BuiltIn/string.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_bitset : std::false_type {};
template <std::size_t N> struct is_std_bitset<std::bitset<N>> : std::true_type {};

} // namespace meta

inline namespace library
{

// slow implementation
EXTERN_CONDITIONAL_SERIALIZATION(Save, bitset, meta::is_std_bitset<T>::value)
{
    auto data = bitset.to_string();
    archive & data;

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, bitset, meta::is_std_bitset<T>::value)
{
    std::string data;
    archive & data;

    bitset = T(data);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_bitset<T>::value)

#endif // SF_BUILT_IN_BITSET_HPP
