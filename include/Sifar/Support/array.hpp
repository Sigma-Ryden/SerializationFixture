#ifndef SIFAR_SUPPORT_ARRAY_HPP
#define SIFAR_SUPPORT_ARRAY_HPP

#include <type_traits> // true_type, false_type

#include <array> // array

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>
#include <Sifar/UnifiedData.hpp>

#include <Sifar/TypeRegistry.hpp>

namespace sifar
{

namespace meta
{

template <typename> struct is_std_array : std::false_type {};
template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

} // namespace meta

namespace library
{

SERIALIZATION_SAVE_LOAD_DATA(array, meta::is_std_array<T>::value)
{
    for (auto& item : array)
        archive & item;

    return archive;
}

} // namespace library

} // namespace sifar

SERIALIZATION_CONDITIONAL_TYPE_REGISTRY(meta::is_std_array<T>::value)

#endif // SIFAR_SUPPORT_ARRAY_HPP
