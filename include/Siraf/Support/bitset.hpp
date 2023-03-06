#ifndef SIRAF_SUPPORT_BITSET_HPP
#define SIRAF_SUPPORT_BITSET_HPP

#include <type_traits> // true_type, false_type

#include <bitset> // bitset

#include <Siraf/Core/TypeRegistry.hpp>
#include <Siraf/ExternSerialization.hpp>

// default array for bitset convertion
#include <Siraf/Support/string.hpp>

namespace siraf
{

namespace meta
{

template <typename> struct is_std_bitset : std::false_type {};
template <std::size_t N>
struct is_std_bitset<std::bitset<N>> : std::true_type {};

} // namespace meta

inline namespace library
{

// slow impl
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

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_bitset<T>::value)

#endif // SIRAF_SUPPORT_BITSET_HPP
