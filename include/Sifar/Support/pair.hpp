#ifndef SIFAR_SUPPORT_PAIR_HPP
#define SIFAR_SUPPORT_PAIR_HPP

#include <type_traits> // true_type, false_type

#include <utility> // pair

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>

#include <Sifar/TypeRegistry.hpp>

namespace sifar
{

namespace meta
{

template <typename> struct is_std_pair : std::false_type {};
template <typename T1, typename T2>
struct is_std_pair<std::pair<T1, T2>> : std::true_type {};

} // namespace meta

namespace library
{

SERIALIZATION_SAVE_DATA(pair, meta::is_std_pair<T>::value)
{
    archive & pair.first & pair.second;

    return archive;
}

SERIALIZATION_LOAD_DATA(pair, meta::is_std_pair<T>::value)
{
    archive & pair.first & pair.second;

    return archive;
}

} // namespace library

} // namespace sifar

SERIALIZATION_TYPE_REGISTRY_IF(meta::is_std_pair<T>::value)

#endif // SIFAR_SUPPORT_PAIR_HPP
