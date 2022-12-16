#ifndef SIRAF_SUPPORT_PAIR_HPP
#define SIRAF_SUPPORT_PAIR_HPP

#include <type_traits> // true_type, false_type

#include <utility> // pair

#include <Siraf/WriteArchive.hpp>
#include <Siraf/ReadArchive.hpp>
#include <Siraf/UnifiedData.hpp>

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

CONDITIONAL_SAVE_LOAD_SERIALIZABLE_TYPE(pair, meta::is_std_pair<T>::value)
{
    archive & pair.first & pair.second;

    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_std_pair<T>::value)

#endif // SIRAF_SUPPORT_PAIR_HPP
