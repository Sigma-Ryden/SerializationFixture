#ifndef SIRAF_SUPPORT_ARRAY_HPP
#define SIRAF_SUPPORT_ARRAY_HPP

#include <type_traits> // true_type, false_type

#include <array> // array

#include <Siraf/WriteArchive.hpp>
#include <Siraf/ReadArchive.hpp>
#include <Siraf/UnifiedData.hpp>

#include <Siraf/Compress.hpp>

#include <Siraf/TypeRegistry.hpp>

namespace siraf
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

} // namespace siraf

CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_std_array<T>::value)

#endif // SIRAF_SUPPORT_ARRAY_HPP
