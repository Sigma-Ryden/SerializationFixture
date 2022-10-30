#ifndef SIFAR_SUPPORT_ARRAY_HPP
#define SIFAR_SUPPORT_ARRAY_HPP

#include <type_traits> // true_type, false_type

#include <array> // array

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>
#include <Sifar/UnifiedData.hpp>

#include <Sifar/Compress.hpp>

#include <Sifar/TypeRegistry.hpp>

namespace sifar
{

namespace meta
{

template <typename> struct is_std_array : std::false_type {};
template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

} // namespace meta

inline namespace library
{

CONDITIONAL_SAVE_LOAD_SERIALIZABLE_TYPE(array, meta::is_std_array<T>::value)
{
    compress::zip(archive, array);
    return archive;
}

} // inline namespace library

} // namespace sifar

CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_std_array<T>::value)

#endif // SIFAR_SUPPORT_ARRAY_HPP
