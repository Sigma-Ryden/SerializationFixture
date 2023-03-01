#ifndef SIRAF_SUPPORT_PAIR_HPP
#define SIRAF_SUPPORT_PAIR_HPP

#include <type_traits> // true_type, false_type

#include <utility> // pair

#include <Siraf/ExternSerialization.hpp>
#include <Siraf/TypeRegistry.hpp>

namespace siraf
{

namespace meta
{

template <typename> struct is_std_pair : std::false_type {};
template <typename T1, typename T2>
struct is_std_pair<std::pair<T1, T2>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, pair, meta::is_std_pair<T>::value)
{
    archive & pair.first & pair.second;
    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_pair<T>::value)

#endif // SIRAF_SUPPORT_PAIR_HPP
